# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Dify Workflow Designer — a CLI tool that takes natural language requirements (text, PDF, CSV, Excel), calls an LLM to design a Dify workflow, and generates a standard Dify DSL YAML file for import into Dify Studio.

## Commands

```bash
# Install dependencies
pip install -r workflow_designer/requirements.txt

# Run CLI
python -m workflow_designer --text "your requirements"
python -m workflow_designer requirements.txt -o output.yml
python -m workflow_designer requirements.pdf --type pdf
python -m workflow_designer --text "..." --interactive  # edit spec before generation

# Environment variables (alternative to CLI args)
export LLM_API_KEY="your-key"
export LLM_API_BASE="https://api.openai.com/v1"
export LLM_MODEL="gpt-4o"
```

## Architecture

The pipeline flows: **Input** → **LLM Design** → **Spec Normalization** → **DSL Build** → **Layout** → **YAML Output**

### Module Layout

```
workflow_designer/
├── __main__.py          # python -m entry point, calls cli.main()
├── cli.py               # CLI arg parsing, API key resolution, user confirmations
├── designer.py           # LLM interaction + spec normalization (the "brain")
├── dsl_builder.py        # Converts normalized spec → full Dify DSL dict → YAML
├── input_parser.py       # Reads text/PDF/CSV/Excel files into plain text
├── layout.py             # BFS-based auto-layout for node positions
├── node_templates.py     # 13 template functions (one per Dify node type)
├── tests/
│   └── sample_requirements.txt
```

### Data Flow

1. **`input_parser.py`** — Parses input files (.txt/.md/.pdf/.csv/.xlsx) into plain text. Auto-detects type from extension.
2. **`designer.py`** — Calls OpenAI-compatible LLM API with a system prompt describing Dify node types and JSON output format. Returns a workflow spec JSON. The `_normalize_spec()` function then fixes common LLM output issues: renumbers node IDs to n0/n1/..., normalizes operators (`equals`→`is`, etc.), converts `{{var}}` references to Dify's `{{#nX.field#}}` syntax, and fixes selector field names to match Dify node output fields.
3. **`dsl_builder.py`** — Converts the normalized spec into a complete Dify DSL dict. Generates unique numeric node IDs, converts each spec node via `node_templates.py` functions, builds edges, assigns layout positions, and replaces spec ID references with real IDs. Outputs via `dsl_to_yaml()` which uses PyYAML with multiline string preservation.
4. **`layout.py`** — BFS-based layout: main chain flows left-to-right, `if-else` false branches shift down by Y_STEP.
5. **`node_templates.py`** — Each `make_*()` function builds a Dify node dict matching the Dify Studio internal format. `NODE_MAKERS` dict maps type strings to functions.

### Key Design Decisions

- **Spec normalization is critical** — LLMs frequently deviate from the expected JSON format. `designer.py:_normalize_spec()` handles ~15 categories of common LLM output issues including operator aliases, selector formats, prompt variable references, and node ID renumbering.
- **Dify-specific variable referencing** — Dify uses `{{#node_id.field_name#}}` syntax (not `{{var}}`). The `_fix_prompt_variables()` function in designer.py builds a variable-to-source-node mapping and converts all references.
- **Node ID mapping** — Spec uses sequential IDs (n0, n1...), but Dify uses timestamp-based numeric IDs. `dsl_builder.py` maintains an `id_mapping` dict throughout the conversion.
- **CLI auto-detects Claude Code settings** — `cli.py:_detect_claude_config()` reads `~/.claude/settings.json` to auto-resolve API keys from Anthropic/DeepSeek/MiniMax configurations.

### Supported Node Types

start, end, llm, code, parameter-extractor, knowledge-retrieval, agent, template-transform, if-else, iteration (with sub-nodes), list-operator, document-extractor
