# Dify Workflow Designer

从自然语言需求自动生成 Dify DSL 工作流文件的 CLI 工具。

## 功能

- **输入灵活**：支持文本、PDF、CSV、Excel 作为需求输入
- **AI 驱动**：调用 LLM 自动分析需求并设计工作流结构
- **自动布局**：根据图结构自动计算节点位置
- **一键导出**：生成标准的 Dify DSL YAML 文件，直接导入 Dify Studio 使用
- **多种节点支持**：start、end、llm、code、parameter-extractor、knowledge-retrieval、agent、template-transform、if-else、iteration、list-operator、document-extractor

## 快速开始

```bash
# 安装依赖
pip install -r workflow_designer/requirements.txt

# 设置 API Key（也可通过 --api-key 参数传入）
export LLM_API_KEY="your-api-key"

# 从文本需求生成工作流
python -m workflow_designer --text "帮我做一个客服自动回复工作流"

# 从需求文件生成
python -m workflow_designer requirements.txt

# 指定输出路径
python -m workflow_designer requirements.txt -o my_workflow.yml
```

## 使用示例

```bash
# 从 PDF 需求文档生成
python -m workflow_designer requirements.pdf --type pdf

# 从 CSV 数据生成
python -m workflow_designer data.csv --type csv

# 指定 LLM 模型
python -m workflow_designer --text "..." --model gpt-4o --api-base https://api.openai.com/v1

# 交互模式（生成前可编辑工作流结构）
python -m workflow_designer --text "..." --interactive

# 跳过确认直接生成
python -m workflow_designer --text "..." --yes
```

## 参数说明

| 参数 | 说明 |
|------|------|
| `source` | 输入文件路径（文本/PDF/CSV/Excel） |
| `--text` | 直接输入需求文本 |
| `--type` | 输入类型：text、pdf、csv、xlsx |
| `-o, --output` | 输出 DSL 文件路径（默认: output.yml） |
| `--api-key` | LLM API Key |
| `--api-base` | API 地址（默认: https://api.openai.com/v1） |
| `--model` | 模型名称 |
| `--interactive` | 生成前允许编辑 workflow spec |
| `-y, --yes` | 跳过确认，直接生成 |

## 环境变量

| 变量 | 说明 |
|------|------|
| `LLM_API_KEY` | API 密钥 |
| `LLM_API_BASE` | API 地址 |
| `LLM_MODEL` | 模型名称 |

工具会自动检测 Claude Code 的配置文件 (`~/.claude/settings.json`)，如果已配置过则可直接使用无需额外设置。

### 支持的工作流节点

| 节点类型 | 说明 |
|---------|------|
| start | 用户输入变量定义 |
| end | 输出变量定义 |
| llm | LLM 调用（支持 system/user prompt、结构化输出） |
| code | Python 代码执行节点 |
| parameter-extractor | 从文本中提取结构化参数 |
| knowledge-retrieval | 知识库检索（RAG） |
| agent | ReAct 智能体（可绑定工具） |
| template-transform | Jinja2 模板转换 |
| if-else | 条件分支 |
| iteration | 循环迭代 |
| list-operator | 列表过滤/操作 |
| document-extractor | 文档内容提取 |

## 生成的工作流导入 Dify

1. 打开 Dify Studio
2. 点击"导入 DSL"
3. 选择生成的 YAML 文件
4. 配置知识库 ID（如有 knowledge-retrieval 节点）和模型供应商等参数

## 项目结构

```
workflow_designer/
├── __init__.py              # 包初始化
├── __main__.py              # python -m 入口
├── cli.py                   # CLI 参数解析、API 配置、交互流程
├── designer.py              # LLM 驱动的工作流设计 + spec 规范化
├── dsl_builder.py           # 将 workflow spec 转换为 Dify DSL YAML
├── input_parser.py          # 多种格式输入解析（text/pdf/csv/xlsx）
├── layout.py                # 节点自动布局算法（BFS、分支偏移）
├── node_templates.py        # 13 种节点类型的模板生成函数
├── requirements.txt         # 依赖
├── README.md                # 本文件
└── tests/
    └── sample_requirements.txt  # 示例需求文件
```

## 工作原理

1. **输入解析**：读取用户提供的需求（文本/PDF/CSV/Excel）
2. **LLM 设计**：调用 LLM 分析需求，生成结构化的 Workflow Spec（JSON）
3. **Spec 规范化**：自动修正 LLM 输出中的格式问题（节点 ID 重编号、运算符映射、变量引用转换等）
4. **DSL 构建**：将规范化后的 Spec 转换为完整的 Dify DSL 结构
5. **自动布局**：基于图拓扑计算节点位置
6. **YAML 输出**：序列化为标准 Dify DSL YAML 文件
