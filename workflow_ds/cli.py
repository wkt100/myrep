"""CLI entry point for the Dify Workflow Designer."""

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Optional

from .designer import design_workflow, preview_workflow
from .dsl_builder import build_dsl, dsl_to_yaml
from .input_parser import parse_input, summarize_content


# Known providers and their OpenAI-compatible mappings
# The api_base must match the actual provider API domain (not the proxy domain).
# Model names are resolved from env vars when possible, with fallback defaults.
PROVIDER_MAP = {
    "deepseek": {
        "api_base": "https://api.deepseek.com/v1",
        "model_fallback": "deepseek-chat",
        "match_domain": "deepseek.com",
    },
    "minimax": {
        "api_base": "https://api.minimaxi.com/v1",
        "model_fallback": "MiniMax-M2.7",
        "match_domain": "minimaxi.com",
    },
}


def _detect_claude_config() -> dict:
    """Detect LLM config from Claude Code's settings.json.

    Returns {"api_key": ..., "api_base": ..., "model": ...} or empty dict.
    """
    claude_settings = Path.home() / ".claude" / "settings.json"
    if not claude_settings.exists():
        return {}

    try:
        with open(claude_settings, "r", encoding="utf-8") as f:
            cfg = json.load(f)
        env = cfg.get("env", {})

        api_key = env.get("ANTHROPIC_AUTH_TOKEN", "")
        base_url = env.get("ANTHROPIC_BASE_URL", "")
        model = env.get("ANTHROPIC_MODEL") or env.get("ANTHROPIC_DEFAULT_SONNET_MODEL", "")

        if not base_url:
            return {}

        # Detect known providers by matching the API domain
        for name, provider in PROVIDER_MAP.items():
            if provider["match_domain"] in base_url.lower():
                # Derive OpenAI-compatible endpoint from the actual base URL
                api_base = base_url.lower().replace("/anthropic", "/v1")
                return {
                    "api_key": api_key,
                    "api_base": api_base,
                    "model": model or provider["model_fallback"],
                }

        # Fallback: try to convert any Anthropic-compatible URL to OpenAI-compatible
        if "anthropic" in base_url.lower():
            oai_endpoint = base_url.lower().replace("/anthropic", "/v1")
            model = model or env.get("ANTHROPIC_DEFAULT_SONNET_MODEL", "gpt-4o")
            return {
                "api_key": api_key,
                "api_base": oai_endpoint,
                "model": model,
            }

        # Standard Anthropic API — not OpenAI-compatible, skip
        return {}

        return {}
    except Exception:
        return {}


def _resolve_api_key(args, claude_cfg: dict) -> str:
    """Resolve API key from args, env, Claude config, or prompt."""
    key = (
        args.api_key
        or os.environ.get("LLM_API_KEY")
        or claude_cfg.get("api_key")
    )
    if not key:
        key = input("Enter LLM API Key: ").strip()
    return key


def _resolve_api_base(args, claude_cfg: dict) -> str:
    """Resolve API base URL."""
    return (
        args.api_base
        or os.environ.get("LLM_API_BASE")
        or claude_cfg.get("api_base")
        or "https://api.openai.com/v1"
    )


def _resolve_model(args, claude_cfg: dict) -> str:
    """Resolve model name."""
    return (
        args.model
        or os.environ.get("LLM_MODEL")
        or claude_cfg.get("model")
        or "gpt-4o"
    )


def _confirm(prompt_text: str, default: Optional[bool] = True) -> bool:
    """Ask user for yes/no confirmation."""
    hint = "Y/n" if default else "y/N"
    resp = input(f"{prompt_text} [{hint}]: ").strip().lower()
    if not resp:
        return default
    return resp[0] == "y"


def _handle_interactive_edit(spec: dict) -> dict:
    """Allow user to edit the workflow spec interactively."""
    print("\n--- 当前 Workflow Spec (JSON) ---")
    print(json.dumps(spec, ensure_ascii=False, indent=2))
    print("---")
    if _confirm("要修改吗?", default=False):
        print("请输入完整的 JSON workflow spec (输入 'END' 结束):")
        lines = []
        while True:
            line = input()
            if line.strip() == "END":
                break
            lines.append(line)
        try:
            edited = json.loads("\n".join(lines))
            return edited
        except json.JSONDecodeError as e:
            print(f"JSON 解析错误: {e}，继续使用原 spec")
    return spec


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Dify Workflow Designer — 从需求自动生成 Dify DSL 工作流",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python -m workflow_designer requirements.txt
  python -m workflow_designer requirements.pdf --model "gpt-4o"
  python -m workflow_designer data.csv --type csv -o workflow.yml
  python -m workflow_designer --text "帮我做一个客服自动回复工作流"
        """,
    )
    parser.add_argument("source", nargs="?", help="输入文件路径或文本")
    parser.add_argument("--text", help="直接输入文本需求（替代 source）")
    parser.add_argument("--type", choices=["text", "pdf", "csv", "xlsx"],
                        help="输入类型（默认自动检测文件扩展名）")
    parser.add_argument("-o", "--output", default="output.yml",
                        help="输出 DSL 文件路径 (默认: output.yml)")
    parser.add_argument("--api-key", help="LLM API Key (或设置 LLM_API_KEY 环境变量)")
    parser.add_argument("--api-base", help="API 地址 (默认: https://api.openai.com/v1)")
    parser.add_argument("--model", default=None,
                        help="模型名称 (默认: gpt-4o, 或 LLM_MODEL 环境变量)")
    parser.add_argument("--interactive", action="store_true",
                        help="生成前允许编辑 workflow spec")
    parser.add_argument("--yes", "-y", action="store_true",
                        help="跳过确认，直接生成")
    return parser


def main():
    parser = build_parser()
    args = parser.parse_args()

    # Resolve input source
    if args.text:
        raw_text = args.text
        source_desc = "直接输入"
    elif args.source:
        raw_text = parse_input(args.source, args.type)
        source_desc = args.source
    else:
        parser.print_help()
        print("\n错误: 请提供输入文件路径或使用 --text 指定需求文本")
        sys.exit(1)

    # Show input summary
    print(f"\n{'='*60}")
    print(f"[Input] Source: {source_desc}")
    print(f"{'='*60}")
    print(summarize_content(raw_text))
    print()

    if not args.yes:
        if not _confirm("Is the input correct?"):
            print("Cancelled")
            sys.exit(0)

    # Resolve API config — auto-detect from Claude Code settings
    claude_cfg = _detect_claude_config()
    api_key = _resolve_api_key(args, claude_cfg)
    if not api_key:
        print("Error: API Key required. Set via --api-key or LLM_API_KEY env var.")
        sys.exit(1)

    api_base = _resolve_api_base(args, claude_cfg)
    model = _resolve_model(args, claude_cfg)

    if claude_cfg:
        print(f"[Config] Auto-detected from Claude Code settings: {api_base} / {model}")

    print(f"\n{'='*60}")
    print(f"[AI] Analyzing requirements with {model}...")
    print(f"{'='*60}")

    try:
        spec = design_workflow(raw_text, api_key, api_base, model)
    except Exception as e:
        print(f"[Error] Workflow design failed: {e}")
        sys.exit(1)

    # Show preview
    print(f"\n{'='*60}")
    print("[Preview] Workflow Design")
    print(f"{'='*60}")
    print(preview_workflow(spec))
    print()

    # Interactive edit
    if args.interactive:
        spec = _handle_interactive_edit(spec)

    # Confirm
    if not args.yes:
        if not _confirm("Generate DSL file?", default=True):
            print("Cancelled")
            sys.exit(0)

    # Build and write DSL
    print(f"\n[Build] Generating DSL...")
    try:
        dsl = build_dsl(spec)
        yaml_str = dsl_to_yaml(dsl)
    except Exception as e:
        print(f"[Error] DSL generation failed: {e}")
        sys.exit(1)

    output_path = args.output
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(yaml_str)

    abs_path = os.path.abspath(output_path)
    print(f"[OK] DSL saved to: {abs_path}")
    print("\nTip: Import into Dify Studio -> Import DSL -> select this file")


if __name__ == "__main__":
    main()
