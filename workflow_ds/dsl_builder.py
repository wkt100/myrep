"""Build complete Dify DSL YAML from a workflow spec."""

import random
import re
import string
import time
from typing import Any, Dict, List, Optional

import yaml

from .layout import assign_node_heights, layout_workflow
from .node_templates import NODE_MAKERS


def _generate_id() -> str:
    """Generate a numeric ID similar to Dify's format (timestamp-based)."""
    base = str(int(time.time() * 1000))
    suffix = "".join(random.choices(string.digits, k=4))
    return base + suffix


def _make_edge(source_id: str, target_id: str, handle: str = "source", is_iteration: bool = False) -> Dict:
    """Build an edge dict."""
    z_index = 1002 if is_iteration else 0
    edge_id = f"{source_id}-{handle}-{target_id}-target"
    return {
        "id": edge_id,
        "source": source_id,
        "target": target_id,
        "sourceHandle": handle,
        "targetHandle": "target",
        "type": "custom",
        "zIndex": z_index,
        "data": {
            "isInIteration": is_iteration,
            "isInLoop": False,
            "sourceType": "",
            "targetType": "",
        },
    }


def _build_feature_config() -> Dict:
    """Build the standard features block."""
    return {
        "file_upload": {
            "enabled": False,
            "allowed_file_types": ["image"],
            "allowed_file_extensions": [".JPG", ".JPEG", ".PNG", ".GIF", ".WEBP", ".SVG"],
            "allowed_file_upload_methods": ["local_file", "remote_url"],
            "fileUploadConfig": {
                "image_file_size_limit": 10,
                "audio_file_size_limit": 50,
                "video_file_size_limit": 100,
                "file_size_limit": 15,
                "file_upload_limit": 20,
                "batch_count_limit": 5,
                "image_file_batch_limit": 10,
                "single_chunk_attachment_limit": 10,
                "workflow_file_upload_limit": 10,
                "attachment_image_file_size_limit": 2,
            },
            "image": {"enabled": False, "number_limits": 3, "transfer_methods": ["local_file", "remote_url"]},
            "number_limits": 3,
        },
        "opening_statement": "",
        "retriever_resource": {"enabled": True},
        "sensitive_word_avoidance": {"enabled": False},
        "speech_to_text": {"enabled": False},
        "suggested_questions": [],
        "suggested_questions_after_answer": {"enabled": False},
        "text_to_speech": {"enabled": False, "language": "", "voice": ""},
    }


def _build_dependencies(workflow_spec: Dict) -> List[Dict]:
    """Build plugin dependencies based on node types used."""
    deps = []
    node_types = {n["type"] for n in workflow_spec["nodes"]}

    if "agent" in node_types:
        deps.append({
            "current_identifier": None,
            "type": "marketplace",
            "value": {
                "marketplace_plugin_unique_identifier": "langgenius/agent:0.0.34@4b41a374567eb7cb226ee2f851513794566956008f1166c63205efc921be72d8",
                "version": None,
            },
        })

    deps.append({
        "current_identifier": None,
        "type": "marketplace",
        "value": {
            "marketplace_plugin_unique_identifier": "langgenius/minimax:0.0.14@38f89e65d3575f79514dfee49b93f858e8fb9301665a36aeffe800a248a48717",
            "version": None,
        },
    })

    return deps


def _replace_text_ids(nodes: List[Dict], id_mapping: Dict[str, str]) -> None:
    """Replace spec IDs (n0, n1...) in {{#nX.field#}} text references with real IDs.

    Modifies nodes in-place.
    """
    replace_map = {}
    for spec_id, real_id in id_mapping.items():
        replace_map[f"#{spec_id}."] = f"#{real_id}."

    def _walk(value: Any) -> Any:
        if isinstance(value, str):
            for old, new in replace_map.items():
                if old in value:
                    value = value.replace(old, new)
            return value
        elif isinstance(value, list):
            return [_walk(v) for v in value]
        elif isinstance(value, dict):
            return {k: _walk(v) for k, v in value.items()}
        return value

    for node in nodes:
        for key in list(node.keys()):
            node[key] = _walk(node[key])


def _map_selectors_in_value(value: Any, id_mapping: Dict[str, str]) -> Any:
    """Recursively walk through node data and map spec node IDs to real IDs.

    Selector arrays have the form ["n0", "field_name"] — the first element is
    always a node ID reference that needs mapping.
    """
    if isinstance(value, list):
        # Check if this is a selector: ["nN", ...]
        if len(value) >= 2 and isinstance(value[0], str) and re.match(r"^n\d+(?:_\w+)?$", value[0]):
            mapped = id_mapping.get(value[0], value[0])
            return [mapped] + [_map_selectors_in_value(v, id_mapping) for v in value[1:]]
        return [_map_selectors_in_value(v, id_mapping) for v in value]
    elif isinstance(value, dict):
        return {k: _map_selectors_in_value(v, id_mapping) for k, v in value.items()}
    return value


def _convert_node(spec_node: Dict, id_mapping: Dict[str, str]) -> Optional[Dict]:
    """Convert a single spec node to a Dify node dict using templates."""
    ntype = spec_node["type"]
    nid = spec_node["id"]
    config = spec_node.get("config", {})
    real_id = id_mapping.get(nid, _generate_id())
    id_mapping[nid] = real_id

    maker = NODE_MAKERS.get(ntype)
    if not maker:
        raise ValueError(f"Unknown node type: {ntype}")

    if ntype == "start":
        return maker(real_id, _map_selectors_in_value(config.get("variables", []), id_mapping))

    elif ntype == "end":
        outputs = _map_selectors_in_value(config.get("outputs", []), id_mapping)
        return maker(real_id, outputs)

    elif ntype == "llm":
        ctx = config.get("context_selector")
        if ctx:
            ctx = _map_selectors_in_value(ctx, id_mapping)
        so = config.get("structured_output")
        return maker(
            real_id,
            prompt_system=config.get("prompt_system", ""),
            prompt_user=config.get("prompt_user", ""),
            model_provider=config.get("model_provider", "langgenius/minimax/minimax"),
            model_name=config.get("model_name", "minimax-m2.7"),
            temperature=config.get("temperature", 0.7),
            context_selector=ctx,
            structured_output=so,
        )

    elif ntype == "code":
        inputs = _map_selectors_in_value(config.get("inputs", []), id_mapping)
        return maker(
            real_id,
            code=config.get("code", "def main():\n    pass"),
            inputs=inputs,
            outputs=config.get("outputs", {"result": {"type": "string"}}),
            language=config.get("language", "python3"),
        )

    elif ntype == "parameter-extractor":
        qs = _map_selectors_in_value(config.get("query_selector", ["n0", "input"]), id_mapping)
        return maker(
            real_id,
            instruction=config.get("instruction", ""),
            parameters=config.get("parameters", []),
            query_selector=qs,
        )

    elif ntype == "knowledge-retrieval":
        qs = _map_selectors_in_value(config.get("query_selector", ["n0", "input"]), id_mapping)
        return maker(
            real_id,
            dataset_ids=config.get("dataset_ids", ["YOUR_DATASET_ID_HERE"]),
            query_selector=qs,
            top_k=config.get("top_k", 4),
        )

    elif ntype == "template-transform":
        vars_data = _map_selectors_in_value(config.get("variables", []), id_mapping)
        return maker(
            real_id,
            template=config.get("template", ""),
            variables=vars_data,
        )

    elif ntype == "if-else":
        conds = _map_selectors_in_value(config.get("conditions", []), id_mapping)
        return maker(
            real_id,
            conditions=conds,
            logical_operator=config.get("logical_operator", "and"),
        )

    elif ntype == "agent":
        ctx = config.get("context_selector")
        if ctx:
            ctx = _map_selectors_in_value(ctx, id_mapping)
        tools = _map_selectors_in_value(config.get("tools"), id_mapping) if config.get("tools") else None
        return maker(
            real_id,
            instruction=config.get("instruction", ""),
            query_template=config.get("query_template", ""),
            context_selector=ctx,
            tools=tools,
        )

    elif ntype == "list-operator":
        vs = _map_selectors_in_value(config.get("variable_selector", []), id_mapping)
        fb = _map_selectors_in_value(config.get("filter_by"), id_mapping) if config.get("filter_by") else None
        return maker(real_id, variable_selector=vs, filter_by=fb, limit=config.get("limit"))

    elif ntype == "document-extractor":
        vs = _map_selectors_in_value(config.get("variable_selector", []), id_mapping)
        return maker(real_id, variable_selector=vs)

    elif ntype == "iteration":
        start_id = id_mapping.get(f"{spec_node['id']}_start", _generate_id())
        id_mapping[f"{spec_node['id']}_start"] = start_id
        it_sel = _map_selectors_in_value(config.get("iterator_selector", []), id_mapping)
        out_sel = _map_selectors_in_value(config.get("output_selector", []), id_mapping)
        return maker(
            real_id,
            start_node_id=start_id,
            iterator_selector=it_sel,
            output_selector=out_sel,
            is_parallel=config.get("is_parallel", True),
            parallel_nums=config.get("parallel_nums", 10),
        )

    elif ntype == "iteration-start":
        parent_id = id_mapping.get(config.get("parent_id", ""), "")
        return maker(real_id, parent_id)

    return None


def build_dsl(workflow_spec: Dict[str, Any]) -> Dict:
    """Convert a workflow spec (from LLM) into a complete Dify DSL dict.

    Returns a dict ready for yaml.dump.
    """
    id_mapping: Dict[str, str] = {}

    all_nodes = []  # Dify node dicts
    all_edges = []
    flat_spec_nodes = []  # non-iteration nodes for layout

    # First pass: assign IDs to all spec nodes
    for spec_node in workflow_spec["nodes"]:
        if spec_node["type"] == "iteration":
            # Assign ID for iteration container
            if spec_node["id"] not in id_mapping:
                id_mapping[spec_node["id"]] = _generate_id()
            # Assign IDs for sub-nodes
            for sn in spec_node.get("config", {}).get("sub_nodes", []):
                if sn["id"] not in id_mapping:
                    id_mapping[sn["id"]] = _generate_id()
            # Assign ID for iteration-start
            start_key = f"{spec_node['id']}_start"
            if start_key not in id_mapping:
                id_mapping[start_key] = _generate_id()
        elif spec_node["id"] not in id_mapping:
            if spec_node["type"] != "iteration-start":
                id_mapping[spec_node["id"]] = _generate_id()

    # Second pass: build nodes
    for spec_node in workflow_spec["nodes"]:
        if spec_node["type"] == "iteration":
            sub_nodes = spec_node.get("config", {}).get("sub_nodes", [])
            iter_parent_id = id_mapping[spec_node["id"]]

            # Create iteration container
            iter_node = _convert_node(spec_node, id_mapping)
            if iter_node:
                all_nodes.append(iter_node)
                flat_spec_nodes.append(spec_node)

            # Create iteration-start
            start_key = f"{spec_node['id']}_start"
            start_spec = {
                "id": start_key,
                "type": "iteration-start",
                "title": "",
                "config": {"parent_id": spec_node["id"]},
            }
            start_node = _convert_node(start_spec, id_mapping)
            if start_node:
                start_node["parentId"] = iter_parent_id
                start_node["position"] = {"x": 24, "y": 68}
                start_node["positionAbsolute"] = {"x": 24, "y": 68}
                all_nodes.append(start_node)

            # Create sub-nodes and edges between them
            prev_inner = start_key
            for sn in sub_nodes:
                sn.setdefault("config", {})
                sn["config"]["parent_id"] = spec_node["id"]
                sn["config"]["iteration_id"] = spec_node["id"]

                # Iteration sub-nodes need their config selectors mapped
                # Temporarily add id_mapping entry for the parent
                inner_node = _convert_node(sn, id_mapping)
                if inner_node:
                    inner_node["parentId"] = iter_parent_id
                    inner_node["position"] = {"x": 128 + len(all_nodes) * 50, "y": 68}
                    inner_node["positionAbsolute"] = {"x": 128 + len(all_nodes) * 50, "y": 68}
                    inner_node.setdefault("height", 87)
                    all_nodes.append(inner_node)

                    all_edges.append(_make_edge(
                        id_mapping.get(prev_inner, prev_inner),
                        inner_node["id"],
                        "source",
                        is_iteration=True,
                    ))
                    prev_inner = sn["id"]

        elif spec_node["type"] == "iteration-start":
            continue
        else:
            node = _convert_node(spec_node, id_mapping)
            if node:
                all_nodes.append(node)
                flat_spec_nodes.append(spec_node)

    # Build edges (non-iteration)
    for e in workflow_spec["edges"]:
        src = id_mapping.get(e["from"], e["from"])
        tgt = id_mapping.get(e["to"], e["to"])
        handle = e.get("handle", "source")

        source_spec = next((n for n in workflow_spec["nodes"] if n["id"] == e["from"]), None)
        target_spec = next((n for n in workflow_spec["nodes"] if n["id"] == e["to"]), None)

        # Skip edges inside iteration
        if source_spec:
            is_inner = source_spec.get("config", {}).get("iteration_id") or source_spec["type"] == "iteration-start"
            if is_inner:
                continue
        if target_spec:
            is_inner = target_spec.get("config", {}).get("iteration_id")
            if is_inner:
                continue

        # Skip edges from iteration (already handled)
        if source_spec and source_spec["type"] == "iteration":
            continue

        all_edges.append(_make_edge(src, tgt, handle))

    # Calculate layout positions using spec IDs, then map to real IDs
    spec_positions = layout_workflow(flat_spec_nodes, workflow_spec["edges"])
    spec_heights = assign_node_heights(flat_spec_nodes)

    # Map spec positions to real node IDs
    for node in all_nodes:
        if "parentId" in node:
            continue  # skip iteration children

        # Find which spec node this real node corresponds to
        real_id = node["id"]
        spec_id = None
        for sid, rid in id_mapping.items():
            if rid == real_id and not sid.endswith("_start"):
                spec_id = sid
                break

        if spec_id and spec_id in spec_positions:
            x, y = spec_positions[spec_id]
            h = spec_heights.get(spec_id, 87)
            node["position"] = {"x": x, "y": y}
            node["positionAbsolute"] = {"x": x, "y": y}
            node["height"] = h
        else:
            node.setdefault("position", {"x": 0, "y": 0})
            node.setdefault("positionAbsolute", {"x": 0, "y": 0})
            node.setdefault("height", 87)

    # 6. Replace spec IDs in prompt text with real IDs
    # (LLM prompts use {{#n0.var#}} format, need to become {{#real_id.var#}})
    _replace_text_ids(all_nodes, id_mapping)

    # Assemble the complete DSL
    viewport = {"x": -50, "y": 0, "zoom": 1}

    dsl = {
        "app": {
            "description": workflow_spec["description"],
            "icon": "🤖",
            "icon_background": "#FFEAD5",
            "icon_type": "emoji",
            "mode": "workflow",
            "name": workflow_spec["name"],
            "use_icon_as_answer_icon": False,
        },
        "dependencies": _build_dependencies(workflow_spec),
        "kind": "app",
        "version": "0.6.0",
        "workflow": {
            "conversation_variables": [],
            "environment_variables": [],
            "features": _build_feature_config(),
            "graph": {
                "edges": all_edges,
                "nodes": all_nodes,
                "viewport": viewport,
            },
            "rag_pipeline_variables": [],
        },
    }

    return dsl


def dsl_to_yaml(dsl: Dict) -> str:
    """Serialize DSL dict to YAML string."""

    def str_repr(dumper, data):
        if "\n" in data:
            return dumper.represent_scalar("tag:yaml.org,2002:str", data, style="|")
        return dumper.represent_scalar("tag:yaml.org,2002:str", data)

    yaml.add_representer(str, str_repr)

    return yaml.dump(
        dsl,
        default_flow_style=False,
        allow_unicode=True,
        sort_keys=False,
        width=120,
    )
