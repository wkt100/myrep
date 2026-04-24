"""Node template functions — build Dify node data dicts from config."""

import uuid
from typing import Any, Dict, List, Optional


def _uid() -> str:
    return str(uuid.uuid4())


def make_start(
    node_id: str,
    variables: List[Dict[str, Any]],
) -> Dict[str, Any]:
    """Build a 'start' (user input) node.

    Each variable::
        {"name": "var", "type": "paragraph", "label": "Label", "required": true}
    """
    var_list = []
    for v in variables:
        entry: Dict[str, Any] = {
            "label": v.get("label", v["name"]),
            "required": v.get("required", True),
            "type": v.get("type", "paragraph"),
            "variable": v["name"],
        }
        if entry["type"] == "paragraph":
            entry.setdefault("max_length", 2000)
        elif entry["type"] == "text-input":
            entry.setdefault("max_length", 256)
        elif entry["type"] == "select":
            entry["options"] = v.get("options", [])
        elif entry["type"] == "file-list":
            entry["allowed_file_types"] = v.get("allowed_file_types", ["document", "image"])
            entry["allowed_file_extensions"] = v.get("allowed_file_extensions", [])
            entry["allowed_file_upload_methods"] = ["local_file", "remote_url"]
        entry.setdefault("default", "")
        entry.setdefault("hint", "")
        entry.setdefault("placeholder", "")
        var_list.append(entry)

    return {
        "data": {
            "type": "start",
            "title": "用户输入",
            "variables": var_list,
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_llm(
    node_id: str,
    prompt_system: str,
    prompt_user: str,
    model_provider: str = "langgenius/minimax/minimax",
    model_name: str = "minimax-m2.7",
    temperature: float = 0.7,
    context_selector: Optional[List[str]] = None,
    structured_output: Optional[Dict] = None,
) -> Dict[str, Any]:
    """Build an LLM node."""
    prompts = [
        {"id": _uid(), "role": "system", "text": prompt_system},
        {"id": _uid(), "role": "user", "text": prompt_user},
    ]
    data: Dict[str, Any] = {
        "type": "llm",
        "title": "LLM",
        "model": {
            "provider": model_provider,
            "name": model_name,
            "mode": "chat",
            "completion_params": {"temperature": temperature},
        },
        "prompt_template": prompts,
        "context": {
            "enabled": context_selector is not None,
            "variable_selector": context_selector or [],
        },
        "vision": {"enabled": False},
        "selected": False,
    }
    if structured_output:
        data["structured_output_enabled"] = True
        data["structured_output"] = {"schema": structured_output}
    return {"data": data, "id": node_id, "type": "custom", "sourcePosition": "right", "targetPosition": "left", "width": 242}


def make_end(
    node_id: str,
    outputs: List[Dict[str, Any]],
) -> Dict[str, Any]:
    """Build an 'end' (output) node.

    Each output::
        {"name": "out", "label": "Output", "value_selector": ["node_id", "field"], "value_type": "string"}
    """
    output_list = []
    for o in outputs:
        output_list.append({
            "variable": o["name"],
            "label": o.get("label", o["name"]),
            "value_selector": o["value_selector"],
            "value_type": o.get("value_type", "string"),
        })
    return {
        "data": {
            "type": "end",
            "title": "输出",
            "outputs": output_list,
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_code(
    node_id: str,
    code: str,
    inputs: List[Dict[str, Any]],
    outputs: Dict[str, Dict[str, str]],
    language: str = "python3",
) -> Dict[str, Any]:
    """Build a code node.

    inputs: [{"variable": "name", "value_selector": ["n1", "field"], "value_type": "string"}]
    outputs: {"result_var": {"type": "string"}}
    """
    return {
        "data": {
            "type": "code",
            "title": "代码执行",
            "code_language": language,
            "code": code,
            "outputs": outputs,
            "variables": [
                {
                    "variable": inp["variable"],
                    "value_selector": inp["value_selector"],
                    "value_type": inp.get("value_type", "string"),
                }
                for inp in inputs
            ],
            "selected": True,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_parameter_extractor(
    node_id: str,
    instruction: str,
    parameters: List[Dict[str, Any]],
    query_selector: List[str],
    model_provider: str = "langgenius/minimax/minimax",
    model_name: str = "minimax-m2.7",
    temperature: float = 0.7,
) -> Dict[str, Any]:
    """Build a parameter-extractor node.

    parameters: [{"name": "field", "type": "string", "description": "...", "required": true}]
    """
    return {
        "data": {
            "type": "parameter-extractor",
            "title": "参数提取器",
            "instruction": instruction,
            "model": {
                "provider": model_provider,
                "name": model_name,
                "mode": "chat",
                "completion_params": {"temperature": temperature},
            },
            "parameters": [
                {
                    "name": p["name"],
                    "type": p.get("type", "string"),
                    "description": p.get("description", ""),
                    "required": p.get("required", True),
                }
                for p in parameters
            ],
            "query": query_selector,
            "reasoning_mode": "prompt",
            "vision": {"enabled": False},
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_knowledge_retrieval(
    node_id: str,
    dataset_ids: List[str],
    query_selector: List[str],
    top_k: int = 4,
    reranking_model: Optional[Dict] = None,
) -> Dict[str, Any]:
    """Build a knowledge-retrieval node.

    reranking_model: {"provider": "...", "model": "..."}
    """
    multiple_config: Dict[str, Any] = {
        "top_k": top_k,
        "reranking_enable": reranking_model is not None,
    }
    if reranking_model:
        multiple_config["reranking_mode"] = "reranking_model"
        multiple_config["reranking_model"] = reranking_model
    else:
        multiple_config["reranking_mode"] = "reranking_model"

    return {
        "data": {
            "type": "knowledge-retrieval",
            "title": "知识检索",
            "dataset_ids": dataset_ids,
            "query_variable_selector": query_selector,
            "retrieval_mode": "multiple",
            "multiple_retrieval_config": multiple_config,
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_template_transform(
    node_id: str,
    template: str,
    variables: List[Dict[str, Any]],
) -> Dict[str, Any]:
    """Build a template-transform node.

    variables: [{"variable": "name", "value_selector": ["n1", "field"], "value_type": "string"}]
    """
    return {
        "data": {
            "type": "template-transform",
            "title": "模板转换",
            "template": template,
            "variables": [
                {
                    "variable": v["variable"],
                    "value_selector": v["value_selector"],
                    "value_type": v.get("value_type", "string"),
                }
                for v in variables
            ],
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_if_else(
    node_id: str,
    conditions: List[Dict[str, Any]],
    logical_operator: str = "and",
) -> Dict[str, Any]:
    """Build an if-else (conditional branch) node.

    Each condition::
        {"comparison_operator": "contains", "value": "...",
         "variable_selector": ["n1", "field"], "var_type": "string"}
    """
    return {
        "data": {
            "type": "if-else",
            "title": "条件分支",
            "cases": [
                {
                    "case_id": "true",
                    "conditions": [
                        {
                            "id": _uid(),
                            "comparison_operator": c["comparison_operator"],
                            "value": c.get("value", ""),
                            "variable_selector": c["variable_selector"],
                            "varType": c.get("var_type", "string"),
                        }
                        for c in conditions
                    ],
                    "logical_operator": logical_operator,
                    "id": "true",
                }
            ],
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_iteration(
    node_id: str,
    start_node_id: str,
    iterator_selector: List[str],
    output_selector: List[str],
    is_parallel: bool = True,
    parallel_nums: int = 10,
) -> Dict[str, Any]:
    """Build an iteration node."""
    return {
        "data": {
            "type": "iteration",
            "title": "迭代",
            "is_parallel": is_parallel,
            "parallel_nums": parallel_nums,
            "iterator_input_type": "array[string]",
            "iterator_selector": iterator_selector,
            "output_selector": output_selector,
            "output_type": "array[string]",
            "start_node_id": start_node_id,
            "flatten_output": True,
            "error_handle_mode": "terminated",
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 785,
    }


def make_iteration_start(
    node_id: str,
    parent_id: str,
) -> Dict[str, Any]:
    """Build an iteration-start marker node (inside iteration)."""
    return {
        "data": {
            "type": "iteration-start",
            "title": "",
            "desc": "",
            "selected": False,
        },
        "id": node_id,
        "parentId": parent_id,
        "type": "custom-iteration-start",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 44,
        "height": 48,
        "draggable": False,
        "selectable": False,
    }


def make_agent(
    node_id: str,
    instruction: str,
    query_template: str,
    context_selector: Optional[List[str]] = None,
    tools: Optional[List[Dict]] = None,
    model_provider: str = "langgenius/minimax/minimax",
    model_name: str = "minimax-m2.7",
) -> Dict[str, Any]:
    """Build an Agent (ReAct) node."""
    return {
        "data": {
            "type": "agent",
            "title": "Agent",
            "agent_strategy_label": "ReAct",
            "agent_strategy_name": "ReAct",
            "agent_strategy_provider_name": "langgenius/agent/agent",
            "plugin_unique_identifier": "langgenius/agent:0.0.34@4b41a374567eb7cb226ee2f851513794566956008f1166c63205efc921be72d8",
            "meta": {"minimum_dify_version": "1.7.0", "version": "0.0.2"},
            "tool_node_version": "2",
            "output_schema": {},
            "agent_parameters": {
                "instruction": {"type": "constant", "value": instruction},
                "query": {"type": "constant", "value": query_template},
                "model": {
                    "type": "constant",
                    "value": {
                        "provider": model_provider,
                        "model": model_name,
                        "model_type": "llm",
                        "mode": "chat",
                        "completion_params": {},
                        "type": "model-selector",
                    },
                },
                "context": {
                    "type": "variable",
                    "value": context_selector or [],
                },
                "tools": {
                    "type": "constant",
                    "value": tools or [],
                },
            },
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


def make_list_operator(
    node_id: str,
    variable_selector: List[str],
    filter_by: Optional[Dict] = None,
    limit: Optional[int] = None,
) -> Dict[str, Any]:
    """Build a list-operator node (filter / sort / limit arrays)."""
    data: Dict[str, Any] = {
        "type": "list-operator",
        "title": "列表操作",
        "variable": variable_selector,
        "var_type": "array[file]",
        "extract_by": {"enabled": False, "serial": "1"},
        "order_by": {"enabled": False, "key": "", "value": "asc"},
        "filter_by": {
            "enabled": False,
            "conditions": [],
        },
        "limit": {"enabled": False, "size": 10},
        "selected": False,
    }
    if filter_by:
        data["filter_by"] = filter_by
    if limit is not None:
        data["limit"] = {"enabled": True, "size": limit}
    return {"data": data, "id": node_id, "type": "custom", "sourcePosition": "right", "targetPosition": "left", "width": 242}


def make_document_extractor(
    node_id: str,
    variable_selector: List[str],
) -> Dict[str, Any]:
    """Build a document-extractor node."""
    return {
        "data": {
            "type": "document-extractor",
            "title": "文档提取器",
            "variable_selector": variable_selector,
            "is_array_file": False,
            "selected": False,
        },
        "id": node_id,
        "type": "custom",
        "sourcePosition": "right",
        "targetPosition": "left",
        "width": 242,
    }


# Mapping from spec type names to maker functions
NODE_MAKERS = {
    "start": make_start,
    "end": make_end,
    "llm": make_llm,
    "code": make_code,
    "parameter-extractor": make_parameter_extractor,
    "knowledge-retrieval": make_knowledge_retrieval,
    "template-transform": make_template_transform,
    "if-else": make_if_else,
    "iteration": make_iteration,
    "iteration-start": make_iteration_start,
    "agent": make_agent,
    "list-operator": make_list_operator,
    "document-extractor": make_document_extractor,
}
