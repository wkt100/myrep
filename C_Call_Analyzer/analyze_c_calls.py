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
    def __init__(self, root_dir):
        self.root_dir = root_dir
        self.c_files = []
        # 修改：按文件路径组织函数定义 {文件路径: {函数名: {行号}}}
        self.function_defs = {}
        # 修改：添加函数名到文件路径的映射 {函数名: 文件路径}
        self.func_to_file = {}
        # 函数调用关系 {调用者: [被调用者列表]}
        self.function_calls = defaultdict(list)
        # 调用者到文件的映射 {调用者: 文件路径}
        self.caller_to_file = {}
        
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
    
    def extract_function_defs(self):
        """提取所有函数定义，保存函数所属的文件名"""
        # 匹配函数定义的正则表达式（简化版）
        func_def_pattern = r'\b(?:\w+\s+)*?(\w+)\s*\([^)]*\)\s*(?:const\s*)?(?:\{|;)'  
        
        print("正在提取函数定义...")
        for file_path in self.c_files:
            # 初始化该文件的函数字典
            if file_path not in self.function_defs:
                self.function_defs[file_path] = {}
                
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    lines = content.split('\n')
                    
                    for line_num, line in enumerate(lines, 1):
                        # 跳过注释行
                        if re.match(r'\s*//', line) or re.match(r'\s*/\*', line):
                            continue
                        
                        # 匹配函数定义
                        matches = re.finditer(func_def_pattern, line)
                        for match in matches:
                            func_name = match.group(1)
                            # 过滤掉C语言关键字和控制流语句
                            if func_name not in C_KEYWORDS:
                                # 保存函数定义信息
                                self.function_defs[file_path][func_name] = {
                                    'line': line_num
                                }
                                # 保存函数名到文件路径的映射
                                self.func_to_file[func_name] = file_path
            except Exception as e:
                print(f"读取文件 {file_path} 时出错: {e}")
        
        # 计算总函数数量
        total_functions = sum(len(funcs) for funcs in self.function_defs.values())
        print(f"共提取到 {total_functions} 个函数定义")
    
    def extract_function_calls(self):
        """提取函数调用关系"""
        print("正在分析函数调用关系...")
        
        # 简化的函数调用匹配模式
        func_call_pattern = r'\b(\w+)\s*\('
        
        for file_path in self.c_files:
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    
                    # 找出所有函数定义块
                    functions = re.split(r'\b(?:\w+\s+)*?(\w+)\s*\([^)]*\)\s*(?:const\s*)?\{', content)
                    
                    for i in range(1, len(functions), 2):
                        func_name = functions[i]
                        func_body = functions[i+1]
                        
                        # 确保调用者也是有效的函数名（不是关键字）
                        if func_name not in C_KEYWORDS:
                            # 保存调用者到文件的映射
                            self.caller_to_file[func_name] = file_path
                            
                            # 找出当前函数体中的所有函数调用
                            calls = re.findall(func_call_pattern, func_body)
                            
                            # 去重
                            unique_calls = list(set(calls))
                            
                            # 过滤掉C语言关键字和控制流语句，只保留实际的函数调用
                            for call in unique_calls:
                                if call in self.func_to_file and call not in C_KEYWORDS:
                                    if call not in self.function_calls[func_name]:
                                        self.function_calls[func_name].append(call)
            
            except Exception as e:
                print(f"分析文件 {file_path} 时出错: {e}")
        
        print(f"共分析到 {len(self.function_calls)} 个函数的调用关系")
    
    def generate_report(self, output_file='call_relations.txt'):
        """生成调用关系报告，按文件分组显示"""
        print(f"正在生成调用关系报告: {output_file}")
        
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
            
            # 按文件分组显示函数定义
            f.write("\n按文件分组的函数定义清单:\n")
            f.write("="*80 + "\n")
            
            # 按文件路径排序
            for file_path in sorted(self.function_defs.keys()):
                relative_path = os.path.relpath(file_path, self.root_dir)
                file_funcs = self.function_defs[file_path]
                
                # 写入文件信息
                f.write(f"\n[{relative_path}] - 包含 {len(file_funcs)} 个函数\n")
                f.write("-"*80 + "\n")
                
                # 写入该文件中的所有函数
                for func_name, info in sorted(file_funcs.items()):
                    f.write(f"  {func_name:<30} | 行号: {info['line']}\n")
            
            # 按文件分组显示函数调用关系
            f.write("\n\n按文件分组的函数调用关系:\n")
            f.write("="*80 + "\n")
            
            if not self.function_calls:
                f.write("未找到函数调用关系。可能是解析规则需要调整。\n")
            else:
                # 按文件组织调用者
                file_to_callers = defaultdict(list)
                for caller in self.function_calls.keys():
                    if caller in self.caller_to_file:
                        file_path = self.caller_to_file[caller]
                        file_to_callers[file_path].append(caller)
                
                # 按文件路径排序并显示
                for file_path in sorted(file_to_callers.keys()):
                    relative_path = os.path.relpath(file_path, self.root_dir)
                    callers = sorted(file_to_callers[file_path])
                    
                    f.write(f"\n[{relative_path}]\n")
                    f.write("-"*80 + "\n")
                    
                    for caller in callers:
                        callees = self.function_calls[caller]
                        f.write(f"  {caller} 调用了:")
                        for callee in sorted(callees):
                            # 找到被调用函数所在的文件
                            if callee in self.func_to_file:
                                callee_file = os.path.relpath(self.func_to_file[callee], self.root_dir)
                            else:
                                callee_file = "(未知)"
                            f.write(f"\n    -> {callee} [{callee_file}]")
                        f.write("\n\n")
        
        # 生成调用图的dot格式
        dot_file = 'call_graph.dot'
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
    
    def analyze(self):
        """执行完整的分析流程"""
        self.find_c_files()
        if not self.c_files:
            print("错误：未找到任何C语言文件！")
            return False
        
        self.extract_function_defs()
        self.extract_function_calls()
        self.generate_report()
        return True

def main():
    parser = argparse.ArgumentParser(description='C语言工程调用关系分析工具')
    parser.add_argument('-d', '--dir', default=os.getcwd(), 
                        help='要分析的目录路径 (默认: 当前目录)')
    
    args = parser.parse_args()
    
    analyzer = CCallAnalyzer(args.dir)
    analyzer.analyze()

if __name__ == '__main__':
    main()