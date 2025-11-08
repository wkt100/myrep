#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
C语言工程调用关系分析工具

功能：
1. 扫描指定目录下所有C语言文件(.c/.h)
2. 解析函数定义和函数调用
3. 构建函数调用关系图
4. 生成调用关系报告，按文件分组显示函数
"""

import os
import re
import argparse
from collections import defaultdict

# C语言关键字和控制流语句列表，这些不是函数
C_KEYWORDS = {
    'if', 'else', 'for', 'while', 'do', 'switch', 'case', 'default',
    'break', 'continue', 'return', 'goto', 'sizeof', 'typedef', 'struct',
    'union', 'enum', 'const', 'static', 'extern', 'auto', 'register',
    'volatile', 'void', 'char', 'short', 'int', 'long', 'float', 'double',
    'signed', 'unsigned'
}

class CCallAnalyzer:
    def __init__(self, root_dir, whitelist=None):
        self.root_dir = root_dir
        # 初始化白名单集合
        self.whitelist = set(whitelist or [])
        self.c_files = []
        # 修改：按文件路径组织函数定义 {文件路径: {函数名: {行号}}}
        self.function_defs = {}
        # 修改：添加函数名到文件路径的映射 {函数名: 文件路径}
        self.func_to_file = {}
        # 函数声明（没有函数体，只有声明） {文件路径: {函数名: {行号}}}
        self.function_declarations = {}
        # 函数调用关系 {调用者: [被调用者列表]}
        self.function_calls = defaultdict(list)
        # 调用者到文件的映射 {调用者: 文件路径}
        self.caller_to_file = {}
        
    def load_whitelist(self, whitelist_file):
        """从文件加载白名单函数"""
        if not whitelist_file or not os.path.exists(whitelist_file):
            return
        
        try:
            with open(whitelist_file, 'r', encoding='utf-8') as f:
                for line in f:
                    # 去除行首尾空白字符和注释
                    func_name = line.strip()
                    if func_name and not func_name.startswith('#'):
                        self.whitelist.add(func_name)
            print(f"从文件 {whitelist_file} 加载了 {len(self.whitelist)} 个白名单函数")
        except Exception as e:
            print(f"加载白名单文件时出错: {e}")
        
    def find_c_files(self):
        """查找所有C语言源文件"""
        print(f"正在搜索 {self.root_dir} 目录下的C语言文件...")
        for dirpath, _, filenames in os.walk(self.root_dir):
            for filename in filenames:
                if filename.endswith(('.c', '.h')):
                    full_path = os.path.join(dirpath, filename)
                    self.c_files.append(full_path)
        print(f"共找到 {len(self.c_files)} 个C语言文件")
        return self.c_files
    
    def extract_function_defs(self, debug=False):
        """提取所有函数定义，保存函数所属的文件名，忽略被注释的函数"""
        # 改进的函数定义匹配正则表达式，更准确地识别函数定义而非函数调用
        # 匹配：[返回类型] [函数名] ([参数列表]) [const] { 或 ;
        # 支持自定义类型如i32_t等
        func_def_pattern = r'\b(?:(?:static|extern|inline|\w+(?:_t)?|struct\s+\w+|union\s+\w+|enum\s+\w+)(?:\s*\*)?\s+)+(\w+)\s*\([^)]*\)\s*(?:const\s*)?\{'  
        
        print("正在提取函数定义...")
        debug_info = []  # 用于调试的信息
        
        for file_path in self.c_files:
            # 初始化该文件的函数字典
            if file_path not in self.function_defs:
                self.function_defs[file_path] = {}
                
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    original_lines = content.split('\n')
                    
                    # 改进的注释检测：使用更精确的方法
                    # 创建一个标记数组，表示哪些位置不在注释内
                    not_in_comment = [True] * len(content)
                    
                    # 标记块注释内的位置（改进：处理嵌套注释标记）
                    block_comment_pattern = r'/\*.*?\*/'
                    for match in re.finditer(block_comment_pattern, content, re.DOTALL):
                        start, end = match.span()
                        for i in range(start, end):
                            if i < len(not_in_comment):
                                not_in_comment[i] = False
                    
                    # 标记行注释内的位置（改进：只标记到行尾）
                    line_comment_pattern = r'//[^\n]*'
                    for match in re.finditer(line_comment_pattern, content):
                        start, end = match.span()
                        for i in range(start, end):
                            if i < len(not_in_comment):
                                not_in_comment[i] = False
                    
                    # 改进的策略：使用滑动窗口方法，检查多行（最多4行）以处理各种函数定义格式
                    # 这样可以处理返回类型、函数名、左大括号分别在不同行的情况
                    
                    max_lines_to_check = 4  # 最多检查4行（返回类型、函数名、参数、左大括号）
                    
                    for line_idx in range(len(original_lines)):
                        # 获取要检查的多行（最多4行）
                        lines_to_check_list = []
                        for i in range(max_lines_to_check):
                            if line_idx + i < len(original_lines):
                                lines_to_check_list.append(original_lines[line_idx + i])
                            else:
                                break
                        
                        if not lines_to_check_list:
                            continue
                        
                        # 合并多行进行检查
                        lines_to_check = '\n'.join(lines_to_check_list)
                        
                        # 去除注释后检查
                        lines_no_comments = self.remove_comments(lines_to_check)
                        
                        # 在多行中查找函数定义
                        # 使用更宽松的模式：允许函数名和左大括号之间有换行和空格
                        func_def_pattern_multiline = r'\b(?:(?:static|extern|inline|\w+(?:_t)?|struct\s+\w+|union\s+\w+|enum\s+\w+|int|void|char|float|double|long|short|unsigned|signed)(?:\s*\*)?\s+)+(\w+)\s*\([^)]*\)\s*(?:const\s*)?\s*\{'
                        matches_in_lines = list(re.finditer(func_def_pattern_multiline, lines_no_comments, re.DOTALL))
                        
                        for match in matches_in_lines:
                            func_name = match.group(1)
                            
                            # 过滤条件
                            if func_name not in C_KEYWORDS and func_name not in self.whitelist:
                                # 获取匹配文本
                                matched_text = match.group(0)
                                
                                # 确保包含左大括号
                                if '{' not in matched_text:
                                    continue
                                
                                # 在检查的行中找到函数名的位置（可能在多行中的任何一行）
                                func_line_idx = line_idx
                                func_pos_in_line = -1
                                
                                for i, check_line in enumerate(lines_to_check_list):
                                    func_pos = check_line.find(func_name + '(')
                                    if func_pos >= 0:
                                        func_line_idx = line_idx + i
                                        func_pos_in_line = func_pos
                                        break
                                
                                if func_pos_in_line >= 0:
                                    # 计算在原始内容中的绝对位置
                                    line_start_pos = sum(len(original_lines[i]) + 1 for i in range(func_line_idx))
                                    absolute_pos = line_start_pos + func_pos_in_line
                                    
                                    # 检查该位置是否在注释外
                                    if absolute_pos < len(not_in_comment) and not_in_comment[absolute_pos]:
                                        # 保存函数定义信息（避免重复）
                                        if func_name not in self.function_defs[file_path]:
                                            self.function_defs[file_path][func_name] = {
                                                'line': func_line_idx + 1
                                            }
                                            self.func_to_file[func_name] = file_path
                                            if debug:
                                                debug_info.append(f"  ✓ 提取: {func_name} (行 {func_line_idx + 1})")
                                        elif debug:
                                            debug_info.append(f"  跳过: {func_name} (行 {func_line_idx + 1}) - 已存在")
                                    elif debug and func_name not in self.function_defs[file_path]:
                                        debug_info.append(f"  跳过: {func_name} (行 {func_line_idx + 1}) - 在注释中")
            except Exception as e:
                print(f"读取文件 {file_path} 时出错: {e}")
                if debug:
                    debug_info.append(f"  错误: {file_path} - {e}")
        
        # 计算总函数数量
        total_functions = sum(len(funcs) for funcs in self.function_defs.values())
        print(f"共提取到 {total_functions} 个函数定义")
        
        # 输出调试信息
        if debug and debug_info:
            print("\n调试信息:")
            for info in debug_info[:50]:  # 只显示前50条
                print(f"  {info}")
            if len(debug_info) > 50:
                print(f"  ... 还有 {len(debug_info) - 50} 条调试信息")
    
    def extract_function_declarations(self):
        """提取函数声明（没有函数体，只有声明，以分号结尾）"""
        # 函数声明匹配模式：返回类型 函数名(参数) ;
        # 注意：要排除函数定义（有函数体的）
        func_decl_pattern = r'\b(?:(?:static|extern|inline|\w+(?:_t)?|struct\s+\w+|union\s+\w+|enum\s+\w+)(?:\s*\*)?\s+)+(\w+)\s*\([^)]*\)\s*;'
        
        print("正在提取函数声明...")
        for file_path in self.c_files:
            # 初始化该文件的函数声明字典
            if file_path not in self.function_declarations:
                self.function_declarations[file_path] = {}
            
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    original_lines = content.split('\n')
                    
                    # 创建一个标记数组，表示哪些位置不在注释内
                    not_in_comment = [True] * len(content)
                    
                    # 标记块注释内的位置
                    block_comment_pattern = r'/\*.*?\*/'
                    for match in re.finditer(block_comment_pattern, content, re.DOTALL):
                        start, end = match.span()
                        for i in range(start, end):
                            if i < len(not_in_comment):
                                not_in_comment[i] = False
                    
                    # 标记行注释内的位置
                    line_comment_pattern = r'//.*$'
                    for match in re.finditer(line_comment_pattern, content, re.MULTILINE):
                        start, end = match.span()
                        for i in range(start, end):
                            if i < len(not_in_comment):
                                not_in_comment[i] = False
                    
                    # 移除注释，用于查找函数声明
                    content_no_comments = self.remove_comments(content)
                    
                    # 先找出所有函数定义的位置，排除它们
                    func_def_pattern = r'\b(?:(?:static|extern|inline|\w+(?:_t)?|struct\s+\w+|union\s+\w+|enum\s+\w+)(?:\s*\*)?\s+)+(\w+)\s*\([^)]*\)\s*(?:const\s*)?\{'
                    def_matches = list(re.finditer(func_def_pattern, content_no_comments, re.DOTALL))
                    defined_funcs = set()
                    for def_match in def_matches:
                        defined_funcs.add(def_match.group(1))
                    
                    # 查找函数声明
                    decl_matches = list(re.finditer(func_decl_pattern, content_no_comments))
                    
                    for match in decl_matches:
                        func_name = match.group(1)
                        # 过滤条件：
                        # 1. 不是C语言关键字
                        # 2. 不在白名单中
                        # 3. 不是函数定义（已经提取过的函数定义）
                        if (func_name not in C_KEYWORDS and 
                            func_name not in self.whitelist and
                            func_name not in defined_funcs):
                            
                            # 找到匹配在原始内容中的大致位置
                            pos_in_no_comments = match.start()
                            approximate_pos = 0
                            current_pos = 0
                            
                            for i, line in enumerate(original_lines):
                                line_no_comments = self.remove_comments(line)
                                if current_pos + len(line_no_comments) >= pos_in_no_comments:
                                    approximate_pos = i + 1
                                    break
                                current_pos += len(line_no_comments) + 1
                            
                            # 检查函数声明是否在注释外
                            func_pattern_in_original = r'\b' + re.escape(func_name) + r'\s*\([^)]*\)\s*;'
                            func_match = re.search(func_pattern_in_original, content)
                            
                            if func_match:
                                func_pos = func_match.start()
                                if func_pos < len(not_in_comment) and not_in_comment[func_pos]:
                                    # 保存函数声明信息（不覆盖函数定义）
                                    if func_name not in self.function_defs.get(file_path, {}):
                                        self.function_declarations[file_path][func_name] = {
                                            'line': approximate_pos if approximate_pos > 0 else 1
                                        }
            
            except Exception as e:
                print(f"读取文件 {file_path} 时出错: {e}")
        
        # 计算总声明数量
        total_declarations = sum(len(decls) for decls in self.function_declarations.values())
        print(f"共提取到 {total_declarations} 个函数声明")
    
    def remove_comments(self, code):
        """移除代码中的注释"""
        # 首先移除块注释 /* ... */
        code = re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)
        # 然后移除行注释 // ...
        code = re.sub(r'//.*$', '', code, flags=re.MULTILINE)
        return code
    
    def extract_function_calls(self):
        """提取函数调用关系，忽略注释中的函数调用"""
        print("正在分析函数调用关系...")
        
        # 函数调用匹配模式
        func_call_pattern = r'\b(\w+)\s*\('
        # 函数定义模式
        func_def_pattern = r'\b(?:\w+\s+)*?(\w+)\s*\([^)]*\)\s*(?:const\s*)?\{'
        
        for file_path in self.c_files:
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    
                    # 创建一个标记数组，表示哪些位置不在注释内
                    not_in_comment = [True] * len(content)
                    
                    # 标记块注释内的位置
                    block_comment_pattern = r'/\*.*?\*/'
                    for match in re.finditer(block_comment_pattern, content, re.DOTALL):
                        start, end = match.span()
                        for i in range(start, end):
                            if i < len(not_in_comment):
                                not_in_comment[i] = False
                    
                    # 标记行注释内的位置
                    line_comment_pattern = r'//.*$'
                    for match in re.finditer(line_comment_pattern, content, re.MULTILINE):
                        start, end = match.span()
                        for i in range(start, end):
                            if i < len(not_in_comment):
                                not_in_comment[i] = False
                    
                    # 查找所有函数定义
                    func_matches = list(re.finditer(func_def_pattern, content))
                    
                    for i, match in enumerate(func_matches):
                        func_name = match.group(1)
                        # 确保调用者也是有效的函数名（不是关键字）
                        if func_name not in C_KEYWORDS:
                            # 检查函数定义是否在注释外
                            func_start = match.start()
                            if func_start < len(not_in_comment) and not_in_comment[func_start]:
                                # 保存调用者到文件的映射
                                self.caller_to_file[func_name] = file_path
                                
                                # 确定函数体的范围
                                start_pos = match.end()
                                # 找到函数体的结束位置（匹配括号）
                                bracket_count = 1
                                end_pos = start_pos
                                
                                while end_pos < len(content) and bracket_count > 0:
                                    if content[end_pos] == '{':
                                        bracket_count += 1
                                    elif content[end_pos] == '}':
                                        bracket_count -= 1
                                    end_pos += 1
                                
                                # 提取函数体
                                func_body = content[start_pos:end_pos-1] if bracket_count == 0 else content[start_pos:]
                                # 移除函数体中的注释
                                func_body_no_comments = self.remove_comments(func_body)
                                
                                # 找出当前函数体中的所有函数调用
                                calls = re.findall(func_call_pattern, func_body_no_comments)
                                
                                # 去重
                                unique_calls = list(set(calls))
                                
                                # 过滤掉C语言关键字、控制流语句和白名单中的函数调用
                                for call in unique_calls:
                                    if call not in C_KEYWORDS and call not in self.whitelist:
                                        if call not in self.function_calls[func_name]:
                                            self.function_calls[func_name].append(call)
            
            except Exception as e:
                print(f"分析文件 {file_path} 时出错: {e}")
        
        print(f"共分析到 {len(self.function_calls)} 个函数的调用关系")
    
    def generate_report(self, output_file='call_relations.txt', dot_file='call_graph.dot'):
        """生成调用关系报告，按文件分组显示，包括没有调用关系的函数"""
        print(f"正在生成调用关系报告: {output_file}")
        
        # 确保输出目录存在
        output_dir = os.path.dirname(output_file)
        if output_dir and not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # 计算总函数数量
        total_functions = sum(len(funcs) for funcs in self.function_defs.values())
        
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("="*80 + "\n")
            f.write("C语言工程函数调用关系分析报告\n")
            f.write("="*80 + "\n\n")
            
            f.write(f"分析目录: {self.root_dir}\n")
            f.write(f"C语言文件数量: {len(self.c_files)}\n")
            f.write(f"函数定义数量: {total_functions}\n")
            f.write(f"有调用关系的函数数量: {len(self.function_calls)}\n\n")
            
            # 按文件分组显示函数定义（有函数体的）
            f.write("\n按文件分组的函数定义清单（仅统计有函数体的函数）:\n")
            f.write("="*80 + "\n")
            
            # 按文件路径排序
            for file_path in sorted(self.function_defs.keys()):
                relative_path = os.path.relpath(file_path, self.root_dir)
                file_funcs = self.function_defs[file_path]
                
                # 写入文件信息
                f.write(f"\n[{relative_path}] - 包含 {len(file_funcs)} 个函数定义\n")
                f.write("-"*80 + "\n")
                
                # 写入该文件中的所有函数，按行号排序
                for func_name, info in sorted(file_funcs.items(), key=lambda x: x[1]['line']):
                    f.write(f"  {func_name:<30} | 行号: {info['line']}\n")
            
            # 按文件分组显示函数声明（没有函数体，只有声明）
            total_declarations = sum(len(decls) for decls in self.function_declarations.values())
            if total_declarations > 0:
                f.write("\n\n按文件分组的函数声明清单（没有函数体，仅有声明）:\n")
                f.write("="*80 + "\n")
                
                # 按文件路径排序
                for file_path in sorted(self.function_declarations.keys()):
                    relative_path = os.path.relpath(file_path, self.root_dir)
                    file_decls = self.function_declarations[file_path]
                    
                    if len(file_decls) > 0:
                        # 写入文件信息
                        f.write(f"\n[{relative_path}] - 包含 {len(file_decls)} 个函数声明\n")
                        f.write("-"*80 + "\n")
                        
                        # 写入该文件中的所有函数声明，按行号排序
                        for func_name, info in sorted(file_decls.items(), key=lambda x: x[1]['line']):
                            f.write(f"  {func_name:<30} | 行号: {info['line']}\n")
            
            # 按文件分组显示函数调用关系
            f.write("\n\n按文件分组的函数调用关系:\n")
            f.write("="*80 + "\n")
            
            # 按文件组织所有函数（包括没有调用关系的）
            file_to_all_funcs = defaultdict(list)
            for func_name in self.func_to_file:
                file_path = self.func_to_file[func_name]
                file_to_all_funcs[file_path].append(func_name)
            
            # 按文件路径排序并显示所有函数
            for file_path in sorted(file_to_all_funcs.keys()):
                relative_path = os.path.relpath(file_path, self.root_dir)
                # 按行号排序函数
                all_funcs = sorted(file_to_all_funcs[file_path], key=lambda x: self.function_defs.get(file_path, {}).get(x, {}).get('line', 0))
                
                f.write(f"\n[{relative_path}]\n")
                f.write("-"*80 + "\n")
                
                for func_name in all_funcs:
                    if func_name in self.function_calls and self.function_calls[func_name]:
                        # 有调用关系的函数
                        f.write(f"  {func_name} 调用了:")
                        for callee in sorted(self.function_calls[func_name]):
                            # 找到被调用函数所在的文件
                            # 注意：被调用函数可以不在当前分析的目录中有定义
                            if callee in self.func_to_file:
                                callee_file = os.path.relpath(self.func_to_file[callee], self.root_dir)
                            else:
                                # 被调用函数未在当前目录中找到定义（可能是外部函数、系统函数或库函数）
                                callee_file = "(未定义)"
                            f.write(f"\n    -> {callee} [{callee_file}]")
                        f.write("\n\n")
                    else:
                        # 没有调用其他函数的函数
                        f.write(f"  {func_name} 调用了: 无\n\n")
            
            if not any(self.function_calls.values()):
                f.write("未找到函数调用关系。可能是解析规则需要调整。\n")
        
        # 确保DOT文件目录存在
        dot_dir = os.path.dirname(dot_file)
        if dot_dir and not os.path.exists(dot_dir):
            os.makedirs(dot_dir)
        
        # 生成调用图的dot格式
        with open(dot_file, 'w', encoding='utf-8') as dot:
            dot.write("digraph CallGraph {\n")
            dot.write("    rankdir=LR;\n")
            dot.write("    node [shape=box];\n\n")
            
            # 按文件对节点进行分组（使用子图）
            file_subgraphs = {}
            for file_path, funcs in self.function_defs.items():
                relative_path = os.path.relpath(file_path, self.root_dir)
                # 生成子图ID
                subgraph_id = f"cluster_{relative_path.replace('/', '_').replace('\\', '_').replace('.', '_')}"
                file_subgraphs[file_path] = subgraph_id
                
                dot.write(f"    subgraph {subgraph_id} {{\n")
                dot.write(f"        label = \"{relative_path}\";\n")
                dot.write(f"        style = filled;\n")
                dot.write(f"        color = lightgrey;\n")
                
                for func_name in funcs:
                    dot.write(f"        \"{func_name}\";\n")
                
                dot.write(f"    }}\n")
            
            # 添加调用关系边
            for caller, callees in self.function_calls.items():
                for callee in callees:
                    dot.write(f"    \"{caller}\" -> \"{callee}\"\n")
            
            dot.write("}\n")
        
        print(f"报告生成完成: {output_file}")
        print(f"DOT格式调用图: {dot_file}")
    
    def analyze(self, output_file='call_relations.txt', dot_file='call_graph.dot', debug=False):
        """执行完整的分析流程"""
        self.find_c_files()
        if not self.c_files:
            print("错误：未找到任何C语言文件！")
            return False
        
        self.extract_function_defs(debug=debug)
        self.extract_function_declarations()
        self.extract_function_calls()
        self.generate_report(output_file, dot_file)
        return True

def main():
    parser = argparse.ArgumentParser(description='C语言工程调用关系分析工具')
    parser.add_argument('-d', '--dir', default=os.getcwd(), 
                        help='要分析的目录路径 (默认: 当前目录)')
    parser.add_argument('-o', '--output', default='call_relations.txt',
                        help='报告输出文件路径 (默认: 当前目录下的call_relations.txt)')
    parser.add_argument('-g', '--graph', default='call_graph.dot',
                        help='DOT调用图输出文件路径 (默认: 当前目录下的call_graph.dot)')
    parser.add_argument('-w', '--whitelist', help='白名单函数文件路径，每行一个函数名')
    parser.add_argument('-f', '--functions', nargs='+', help='直接指定要忽略的函数名列表')
    parser.add_argument('--debug', action='store_true', help='启用调试模式，输出详细的提取信息')
    
    args = parser.parse_args()
    
    # 合并命令行指定的函数和白名单文件中的函数
    whitelist = []
    if args.functions:
        whitelist.extend(args.functions)
    
    analyzer = CCallAnalyzer(args.dir, whitelist)
    
    # 从文件加载白名单（如果指定了文件）
    if args.whitelist:
        analyzer.load_whitelist(args.whitelist)
    
    if analyzer.whitelist:
        print(f"总计忽略 {len(analyzer.whitelist)} 个白名单函数")
    
    analyzer.analyze(args.output, args.graph, debug=args.debug)

if __name__ == '__main__':
    main()