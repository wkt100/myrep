"""LLM-powered workflow design — analyze requirements and output a structured spec."""

import json
import os
import re
from typing import Any, Dict, List, Optional

import requests


SYSTEM_PROMPT = """You are a Dify workflow designer. Dify is an open-source LLM application platform.

Your task: Design a Dify workflow based on the user's requirements and output a strict JSON specification.

## Available Node Types

1. **start** — Input variables. Config: {"variables": [{"name": "...", "type": "paragraph|text-input|select|file-list", "label": "...", "required": true}]}
2. **end** — Output variables. Config: {"outputs": [{"name": "result", "label": "...", "value_selector": ["nX", "field"], "value_type": "string"}]}
3. **llm** — LLM call. Config: {"prompt_system": "system prompt", "prompt_user": "user prompt with {{variables}}", "context_selector": ["nX", "field"] (optional), "structured_output": {JSON schema} (optional)}
4. **code** — Python execution. Config: {"code": "def main(var1: str) -> dict:\\n    return {\\"result\\": var1}", "inputs": [{"variable": "var1", "value_selector": ["nX", "field"], "value_type": "string"}], "outputs": {"result": {"type": "string"}}}
5. **parameter-extractor** — Extract structured fields. Config: {"instruction": "...", "parameters": [{"name": "...", "type": "string", "description": "..."}], "query_selector": ["nX", "field"]}
6. **knowledge-retrieval** — RAG. Config: {"dataset_ids": ["YOUR_DATASET_ID_HERE"], "query_selector": ["nX", "field"], "top_k": 4}
7. **agent** — ReAct agent. Config: {"instruction": "...", "query_template": "...", "context_selector": ["nX", "field"] (optional)}
8. **template-transform** — Jinja2 template. Config: {"template": "{% for ... %}", "variables": [{"variable": "name", "value_selector": ["nX", "field"], "value_type": "string"}]}
9. **if-else** — Conditional branch. Config: {"conditions": [{"comparison_operator": "is|not empty|contains|not contains|empty|not in|in|start with|end with|=", "value": "...", "variable_selector": ["nX", "field"], "var_type": "string"}], "logical_operator": "and"}
10. **iteration** — Process array. Config: {"iterator_selector": ["nX", "field"], "output_selector": ["nX", "field"], "is_parallel": true, "sub_nodes": [list of node specs with IDs like "n5_inner_0", "n5_inner_1"]}

## CRITICAL FORMAT RULES

- Node IDs MUST be "n0", "n1", "n2", "n3", ... Never use descriptive IDs.
- n0 is always the start node.
- The last node is always type "end".
- Edges format: {"from": "n0", "to": "n1", "handle": "source"} (or "true"/"false" for if-else branches)
- All value_selector / variable_selector / query_selector must be ["nX", "field_name"]
- For iteration sub_nodes, use "n5_inner_0", "n5_inner_1" format.

## OUTPUT FORMAT - STRICT JSON ONLY

{
  "name": "WorkflowName",
  "description": "Short description",
  "inputs": [{"name": "var", "type": "paragraph", "label": "Label", "required": true}],
  "outputs": [{"name": "result", "label": "Output", "value_selector": ["nX", "field"], "value_type": "string"}],
  "nodes": [
    {"id": "n0", "type": "start", "title": "User Input", "config": {"variables": [...]}},
    {"id": "n1", "type": "...", "title": "...", "config": {...}},
    ...
  ],
  "edges": [
    {"from": "n0", "to": "n1", "handle": "source"},
    {"from": "n1", "to": "n2", "handle": "source"}
  ]
}

Respond with ONLY the JSON. No markdown fences, no explanations."""


def call_llm(
    prompt: str,
    api_key: str,
    api_base: str = "https://api.openai.com/v1",
    model: str = "gpt-4o",
    temperature: float = 0.3,
) -> str:
    """Call OpenAI-compatible chat completion API."""
    headers = {
        "Authorization": f"Bearer {api_key}",
        "Content-Type": "application/json",
    }
    payload = {
        "model": model,
        "messages": [
            {"role": "system", "content": SYSTEM_PROMPT},
            {"role": "user", "content": prompt},
        ],
        "temperature": temperature,
    }

    resp = requests.post(
        f"{api_base.rstrip('/')}/chat/completions",
        headers=headers,
        json=payload,
        timeout=180,
    )

    if not resp.ok:
        raise RuntimeError(
            f"API error ({resp.status_code}): {resp.text[:500]}"
        )

    try:
        data = resp.json()
    except json.JSONDecodeError as e:
        raise RuntimeError(
            f"API returned non-JSON response ({resp.status_code}): {resp.text[:500]}"
        ) from e

    try:
        content = data["choices"][0]["message"]["content"]
    except (KeyError, IndexError, TypeError) as e:
        raise RuntimeError(
            f"Unexpected API response structure: {json.dumps(data, ensure_ascii=False)[:500]}"
        ) from e

    return content


def _try_parse_json(text: str) -> Optional[Dict]:
    """Try to parse JSON with common fixes for LLM output issues."""
    # Try as-is first
    try:
        return json.loads(text)
    except json.JSONDecodeError:
        pass

    # Fix 1: strip trailing commas before ] or }
    fixed = re.sub(r",(\s*[}\]])", r"\1", text)
    try:
        return json.loads(fixed)
    except json.JSONDecodeError:
        pass

    # Fix 2: replace single quotes with double quotes (outside of strings)
    # Simple approach: replace ' with " for common patterns
    fixed = re.sub(r"'([^'\\]*)'", lambda m: '"' + m.group(1) + '"', text)
    try:
        return json.loads(fixed)
    except json.JSONDecodeError:
        pass

    return None


def _extract_json(text: str) -> str:
    """Extract JSON from LLM response.

    Handles:
    - MiniMax-style <think>...</think> thinking tags
    - Markdown code fences (```json ... ```)
    - Plain JSON without fences
    - Attempts to fix common JSON deviations (trailing commas, single quotes)
    """
    text = text.strip()

    # MiniMax reasoning: content is often <think><thinking></think><code_fence>JSON<code_fence>
    # Find the last </think> and take everything after it
    last_think_close = text.rfind("</think>")
    if last_think_close != -1:
        text = text[last_think_close + len("</think>"):].strip()

    # Try to extract JSON from markdown code fences first
    fences = list(re.finditer(r"```(?:json)?\s*\n?", text))
    if fences:
        # Use the last fence pair
        start = fences[-1].start()
        end = text.rfind("```")
        if end > start:
            text = text[start:end].strip()
            text = re.sub(r"^```(?:json)?\s*", "", text)
            text = re.sub(r"\s*```$", "", text)
            text = text.strip()
            try:
                json.loads(text)  # verify
                return text
            except json.JSONDecodeError:
                pass  # fall through

    # If no fences or fences didn't yield valid JSON, try the whole text
    # Strip fences from the whole thing
    text = re.sub(r"^```(?:json)?\s*", "", text)
    text = re.sub(r"\s*```$", "", text)

    return text.strip()


def _normalize_spec(spec: dict) -> dict:
    """Normalize an LLM-generated spec into the expected format.

    Handles common deviations from the spec format.
    """
    # Set missing top-level fields
    spec.setdefault("name", "MyWorkflow")
    spec.setdefault("description", "Generated Dify workflow")
    spec.setdefault("inputs", [])
    spec.setdefault("outputs", [])
    spec.setdefault("nodes", [])
    spec.setdefault("edges", [])

    # 1. Renumber node IDs to n0, n1, n2...
    node_id_map = {}
    new_nodes = []
    for i, node in enumerate(spec.get("nodes", [])):
        old_id = node.get("id", f"old_{i}")
        new_id = f"n{i}"
        node_id_map[old_id] = new_id
        node["id"] = new_id
        node.setdefault("title", node.get("type", "").capitalize())
        new_nodes.append(node)
    spec["nodes"] = new_nodes

    # 2. Fix edge format — map old IDs to new IDs, normalize handle/branch
    new_edges = []
    for edge in spec.get("edges", []):
        from_id = node_id_map.get(edge.get("from", ""), edge.get("from", ""))
        to_id = node_id_map.get(edge.get("to", ""), edge.get("to", ""))
        handle = edge.get("handle") or edge.get("branch", "source")
        if handle is True:
            handle = "true"
        elif handle is False:
            handle = "false"
        new_edges.append({"from": from_id, "to": to_id, "handle": handle})
    spec["edges"] = new_edges

    # 3. Fix common config issues
    for node in spec["nodes"]:
        config = node.get("config", {})
        if not config:
            continue

        # Fix selector formats: {"type": "variable", "value": ["nX", "field"]} -> ["nX", "field"]
        for key in ("query_selector", "iterator_selector", "output_selector"):
            if key in config and isinstance(config[key], dict):
                config[key] = config[key].get("value", config[key])

        # Fix code node inputs dict -> list
        if node["type"] == "code":
            inputs = config.get("inputs", [])
            if isinstance(inputs, dict):
                new_inputs = []
                for k, v in inputs.items():
                    if isinstance(v, str) and "{{" in v:
                        # Extract variable reference like {{nX.field}}
                        m = re.search(r"\{\{(\w+)\.(\w+)\}\}", v)
                        if m:
                            ref_id = node_id_map.get(m.group(1), m.group(1))
                            new_inputs.append({
                                "variable": k,
                                "value_selector": [ref_id, m.group(2)],
                                "value_type": "string",
                            })
                        else:
                            new_inputs.append({
                                "variable": k,
                                "value_selector": [ref_id, k],
                                "value_type": "string",
                            })
                    elif isinstance(v, list) and len(v) >= 2:
                        new_inputs.append({
                            "variable": k,
                            "value_selector": v,
                            "value_type": "string",
                        })
                config["inputs"] = new_inputs
            outputs = config.get("outputs", {})
            if isinstance(outputs, list):
                new_outputs = {}
                for o in outputs:
                    if isinstance(o, str):
                        new_outputs[o] = {"type": "string"}
                config["outputs"] = new_outputs or {"result": {"type": "string"}}

        # Fix template-transform variables: list of strings -> list of objects
        if node["type"] == "template-transform":
            variables = config.get("variables", [])
            if variables and isinstance(variables[0], str):
                new_vars = []
                for v in variables:
                    if isinstance(v, str) and "." in v:
                        parts = v.split(".")
                        ref_id = node_id_map.get(parts[0], parts[0])
                        new_vars.append({
                            "variable": parts[-1],
                            "value_selector": [ref_id, parts[-1]],
                            "value_type": "string",
                        })
                    else:
                        new_vars.append({
                            "variable": str(v),
                            "value_selector": ["n0", str(v)],
                            "value_type": "string",
                        })
                config["variables"] = new_vars

        # Fix end node outputs
        if node["type"] == "end":
            outputs = config.get("outputs", [])
            if outputs and isinstance(outputs[0], dict):
                for o in outputs:
                    if "value" in o and "value_selector" not in o:
                        val = o["value"]
                        if isinstance(val, str) and "{{" in val:
                            m = re.search(r"\{\{(\w+)\.(\w+)\}\}", val)
                            if m:
                                ref_id = node_id_map.get(m.group(1), m.group(1))
                                o["value_selector"] = [ref_id, m.group(2)]
                                o["value_type"] = "string"
                            del o["value"]

        # Fix if-else conditions
        if node["type"] == "if-else":
            conditions = config.get("conditions", [])
            for c in conditions:
                # Normalize variable -> variable_selector
                if "variable" in c and "variable_selector" not in c:
                    val = c["variable"]
                    if isinstance(val, str) and "{{" in val:
                        m = re.search(r"\{\{(\w+)\.(\w+)\}\}", val)
                        if m:
                            ref_id = node_id_map.get(m.group(1), m.group(1))
                            c["variable_selector"] = [ref_id, m.group(2)]
                    elif isinstance(val, str) and "." in val:
                        parts = val.split(".")
                        ref_id = node_id_map.get(parts[0], parts[0])
                        c["variable_selector"] = [ref_id, parts[1]]
                    del c["variable"]

                # Normalize operator -> comparison_operator
                op = c.get("operator") or c.get("comparison_operator", "")
                op_map = {
                    "is_not_empty": "not empty",
                    "is_empty": "empty",
                    "not_empty": "not empty",
                    "not-empty": "not empty",
                    "is-not-empty": "not empty",
                    "is-empty": "empty",
                    "equals": "is",
                    "equal": "is",
                    "not_equals": "is not",
                    "not-equals": "is not",
                    "not equals": "is not",
                    "==": "is",
                    "!=": "is not",
                    "=": "is",
                    "contains": "contains",
                    "not contains": "not contains",
                    "not_contains": "not contains",
                    "in": "in",
                    "not in": "not in",
                    "not_in": "not in",
                    "start with": "start with",
                    "starts with": "start with",
                    "start_with": "start with",
                    "end with": "end with",
                    "ends with": "end with",
                    "end_with": "end with",
                    ">": ">",
                    "<": "<",
                    ">=": "\u2265",
                    "<=": "\u2264",
                }
                c["comparison_operator"] = op_map.get(op, op)
                if "operator" in c:
                    del c["operator"]
                c.setdefault("var_type", "string")

        # Fix query_selector that uses template syntax
        qs = config.get("query_selector", [])
        if isinstance(qs, list) and len(qs) == 1 and "{{" in str(qs[0]):
            m = re.search(r"\{\{(\w+)\.(\w+)\}\}", str(qs[0]))
            if m:
                ref_id = node_id_map.get(m.group(1), m.group(1))
                config["query_selector"] = [ref_id, m.group(2)]

    # 4. Fix variable references in prompt text: {{var}} -> {{#nX.var#}}
    # Dify uses {{#node_id.field_name#}} syntax, not {{field_name}}
    _fix_prompt_variables(spec)

    # 5. Fix output field names in selectors based on source node type
    _fix_selector_fields(spec)

    return spec


def _fix_prompt_variables(spec: dict) -> None:
    """Convert {{var}} to {{#nX.var#}} in prompt templates.

    Dify uses {{#node_id.field_name#}} syntax to reference variables from other nodes.
    """
    # Build variable-to-source mapping
    # Maps variable_name -> (node_id, field_name)
    OUTPUT_FIELDS = {
        "llm": "text",
        "knowledge-retrieval": "result",
        "agent": "text",
        "template-transform": "output",
        "document-extractor": "text",
        "list-operator": "result",
    }
    var_source = {}

    for node in spec["nodes"]:
        if node["type"] == "start":
            for v in node.get("config", {}).get("variables", []):
                var_source[v["name"]] = (node["id"], v["name"])
        elif node["type"] == "parameter-extractor":
            for p in node.get("config", {}).get("parameters", []):
                var_source[p["name"]] = (node["id"], p["name"])
        elif node["type"] == "llm":
            var_source["text"] = (node["id"], "text")
            # Also map common aliases LLM might use for the text output
            var_source["reply"] = (node["id"], "text")
            var_source["response"] = (node["id"], "text")
            var_source["answer"] = (node["id"], "text")
            var_source["output"] = (node["id"], "text")
        elif node["type"] == "knowledge-retrieval":
            var_source["result"] = (node["id"], "result")
            var_source["knowledge"] = (node["id"], "result")
            var_source["context"] = (node["id"], "result")
            var_source["knowledge_results"] = (node["id"], "result")
            var_source["knowledge_result"] = (node["id"], "result")
            var_source["search_results"] = (node["id"], "result")
            var_source["retrieved_docs"] = (node["id"], "result")
            var_source["docs"] = (node["id"], "result")
        elif node["type"] == "code":
            for k in node.get("config", {}).get("outputs", {}):
                var_source[k] = (node["id"], k)
        elif node["type"] == "agent":
            var_source["text"] = (node["id"], "text")
            var_source["output"] = (node["id"], "text")

    def _fix_text(text: str) -> str:
        """Replace {{var}} with {{#node_id.field#}}."""
        def _replacer(m):
            var_name = m.group(1)
            src = var_source.get(var_name)
            if src:
                return "{{#" + src[0] + "." + src[1] + "#}}"
            return m.group(0)  # keep as-is if unknown
        return re.sub(r"\{\{(\w+)\}\}", _replacer, text)

    for node in spec["nodes"]:
        config = node.get("config", {})
        if node["type"] == "llm":
            for prompt in config.get("prompt_template", []):
                if isinstance(prompt, dict) and "text" in prompt:
                    prompt["text"] = _fix_text(prompt["text"])
            # Also fix directly specified prompts
            for key in ("prompt_system", "prompt_user"):
                if key in config and isinstance(config[key], str):
                    config[key] = _fix_text(config[key])
        elif node["type"] == "agent":
            for key in ("instruction", "query_template"):
                if key in config and isinstance(config[key], str):
                    config[key] = _fix_text(config[key])
        elif node["type"] == "template-transform":
            # template-transform uses Jinja2 {{variable_name}} syntax, NOT {{#nX.field#}}.
            # The variables array maps variable_name -> value_selector.
            # So we need to build reverse mapping: node_id.field -> variable_name.
            node_var_map = {}
            for v in node.get("config", {}).get("variables", []):
                vs = v.get("value_selector", [])
                if len(vs) >= 2:
                    node_var_map[(vs[0], vs[1])] = v["variable"]
            if "template" in config and isinstance(config["template"], str):
                template = config["template"]

                def _fix_template_ref(m):
                    """Convert {{#nX.field#}} to {{variable_name}}."""
                    inner = m.group(1)  # e.g. "n0.customer_name"
                    parts = inner.rsplit(".", 1)
                    if len(parts) == 2:
                        nid, field = parts
                        var = node_var_map.get((nid, field))
                        if var:
                            return "{{" + var + "}}"
                    return m.group(0)  # keep as-is (e.g. {{reply_content}} already Jinja2)

                config["template"] = re.sub(r"\{\{\#([^#]+)\#\}\}", _fix_template_ref, template)


# Correct Dify output field names for each node type
NODE_OUTPUT_FIELDS = {
    "llm": "text",                     # LLM output field
    "template-transform": "output",    # Template transform output field
    "knowledge-retrieval": "result",   # Knowledge retrieval output field
    "agent": "text",                   # Agent output field
    "document-extractor": "text",      # Document extractor output field
    "list-operator": "result",         # List operator output field
    "start": None,                     # No fixed output field
    "end": None,
    "if-else": None,
    "iteration": None,                 # Depends on output_selector config
}


def _fix_selector_fields(spec: dict) -> None:
    """Walk all selectors and fix field names to match Dify node output fields.

    E.g. if a selector references template-transform node with field "result",
    correct it to "output" — because Dify's template-transform always outputs to "output".
    """
    # Build map: node_id -> node info
    node_map = {}
    for node in spec["nodes"]:
        node_map[node["id"]] = node
        # For code nodes, build valid output field names from config
        if node["type"] == "code":
            outputs_config = node.get("config", {}).get("outputs", {})
            if isinstance(outputs_config, dict):
                node["_valid_outputs"] = set(outputs_config.keys())
        # For parameter-extractor, valid outputs are parameter names
        elif node["type"] == "parameter-extractor":
            params = node.get("config", {}).get("parameters", [])
            node["_valid_outputs"] = {p["name"] for p in params}
        # For iteration, valid output is what output_selector points to
        elif node["type"] == "iteration":
            out_sel = node.get("config", {}).get("output_selector", [])
            if len(out_sel) >= 2:
                node["_valid_outputs"] = {out_sel[1]}
        else:
            node["_valid_outputs"] = set()

    # Recursively walk the spec and fix selectors
    def _walk(value: Any) -> Any:
        if isinstance(value, list):
            # Check if this is a selector: ["nX", "field_name"]
            if (len(value) == 2 and isinstance(value[0], str)
                    and value[0].startswith("n") and value[0][1:].isdigit()):
                src_id = value[0]
                field = value[1]
                if isinstance(field, str) and src_id in node_map:
                    src_node = node_map[src_id]
                    src_type = src_node.get("type", "")
                    correct_field = NODE_OUTPUT_FIELDS.get(src_type)
                    if correct_field is not None and field != correct_field:
                        # Check if this node has dynamic valid outputs
                        valid_outputs = src_node.get("_valid_outputs", set())
                        if field not in valid_outputs:
                            value[1] = correct_field
                return value
            return [_walk(v) for v in value]
        elif isinstance(value, dict):
            return {k: _walk(v) for k, v in value.items()}
        return value

    # Walk all nodes' config and edges
    for node in spec["nodes"]:
        node["config"] = _walk(node.get("config", {}))

    # Walk outputs list at top level
    spec["outputs"] = _walk(spec.get("outputs", []))


def design_workflow(
    requirements: str,
    api_key: str,
    api_base: str = "https://api.openai.com/v1",
    model: str = "gpt-4o",
) -> Dict[str, Any]:
    """Analyze requirements and return a structured workflow specification.

    Returns:
        Dict with keys: name, description, inputs, outputs, nodes, edges
    """
    user_prompt = (
        "Design a Dify workflow for the following requirements:\n\n"
        f"{requirements}\n\n"
        "Output ONLY the JSON workflow specification. No explanations."
    )

    raw = call_llm(user_prompt, api_key, api_base, model)
    raw = _extract_json(raw)
    spec = _try_parse_json(raw)
    if spec is None:
        raise ValueError(f"LLM returned invalid JSON (tried auto-fix): {raw[:200]}")

    # Normalize the spec (fix format deviations)
    spec = _normalize_spec(spec)

    # Fill in missing optional fields
    spec.setdefault("name", "MyWorkflow")
    spec.setdefault("description", "Generated Dify workflow")
    spec.setdefault("inputs", [])
    spec.setdefault("outputs", [])

    # Validate required keys
    required = {"name", "description", "inputs", "outputs", "nodes", "edges"}
    missing = required - set(spec.keys())
    if missing:
        raise ValueError(f"LLM response missing keys: {missing}")

    # Validate node IDs
    for i, node in enumerate(spec["nodes"]):
        expected = f"n{i}"
        if node["id"] != expected:
            # Fix ID
            node["id"] = expected

    # Validate edges reference valid nodes
    node_ids = {n["id"] for n in spec["nodes"]}
    for e in spec["edges"]:
        if e.get("from") not in node_ids:
            raise ValueError(f"Edge references unknown node: from={e.get('from')}")
        if e.get("to") not in node_ids:
            raise ValueError(f"Edge references unknown node: to={e.get('to')}")

    # Ensure first node is start and last is end
    if spec["nodes"]:
        if spec["nodes"][0]["type"] != "start":
            spec["nodes"].insert(0, {
                "id": "n0",
                "type": "start",
                "title": "User Input",
                "config": {"variables": spec.get("inputs", [])},
            })
            # Renumber
            for i, node in enumerate(spec["nodes"]):
                node["id"] = f"n{i}"

    return spec


def preview_workflow(spec: Dict[str, Any]) -> str:
    """Generate a human-readable preview of the workflow spec."""
    lines = [
        f"== {spec['name']} ==",
        f"   {spec['description']}",
        "",
        "Inputs:",
    ]
    for inp in spec["inputs"]:
        lines.append(f"  - {inp['label']} ({inp['type']}){' *required' if inp.get('required') else ''}")

    lines.append("")
    lines.append("Node chain:")

    node_map = {n["id"]: n for n in spec["nodes"]}
    adj = {}
    for e in spec["edges"]:
        adj.setdefault(e["from"], []).append(e)

    visited = set()

    def walk(nid: str, indent: int = 0):
        if nid in visited:
            return
        visited.add(nid)
        node = node_map.get(nid)
        if not node:
            return
        prefix = "  " * indent
        label = f"{prefix}- [{node['id']}] {node['title']} ({node['type']})"
        lines.append(label)

        if node["type"] == "iteration":
            sub_nodes = node.get("config", {}).get("sub_nodes", [])
            for sn in sub_nodes:
                lines.append(f"{prefix}   -> [{sn['id']}] {sn['title']} ({sn['type']})")

        for edge in adj.get(nid, []):
            walk(edge["to"], indent)

    if "n0" in node_map:
        walk("n0")

    lines.append("")
    lines.append(f"Outputs: {len(spec['outputs'])}")
    for out in spec["outputs"]:
        lines.append(f"  - {out['label']}")

    return "\n".join(lines)
