#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
函数对比工具
对比 funs.txt 文件中的函数列表与程序分析结果的差异
"""

import os
import re
import argparse
from collections import defaultdict

def parse_funs_file(funs_file):
    """解析 funs.txt 文件，提取文件名和函数列表"""
    file_functions = {}
    current_file = None
    
    try:
        with open(funs_file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.rstrip('\r\n')  # 保留行首空白，只去掉行尾
                if not line.strip():
                    continue
                
                # 使用制表符分割
                parts = line.split('\t')
                
                # 检查是否是文件行：格式为 编号\t文件名（函数数量）\t函数名
                # 或者检查是否包含中文括号
                if len(parts) >= 2 and parts[1]:
                    # 检查是否包含括号（可能是中文全角括号或英文括号）
                    has_brackets = '（' in parts[1] or '）' in parts[1] or '(' in parts[1] or ')' in parts[1]
                    
                    if has_brackets:
                        # 新文件行
                        filename_part = parts[1].strip()
                        # 提取文件名（去掉括号和函数数量）
                        # 匹配中文括号或英文括号
                        match = re.search(r'^(.+?)[（(]', filename_part)
                        if match:
                            filename = match.group(1).strip()
                            current_file = filename
                            
                            # 提取函数名（如果有）
                            if len(parts) >= 3 and parts[2].strip():
                                func_name = parts[2].strip()
                                if current_file not in file_functions:
                                    file_functions[current_file] = []
                                file_functions[current_file].append(func_name)
                        continue
                
                # 检查是否是续行（函数名行）
                # 格式：\t\t函数名 或者 \t函数名
                if len(parts) >= 3:
                    # 前两个部分是空的或只有空白
                    if (not parts[0].strip() and not parts[1].strip() and parts[2].strip()):
                        func_name = parts[2].strip()
                        if func_name and current_file:
                            if current_file not in file_functions:
                                file_functions[current_file] = []
                            file_functions[current_file].append(func_name)
                        continue
                    # 或者只有两个部分，第一部分为空，第二部分是函数名
                    elif len(parts) == 2 and not parts[0].strip() and parts[1].strip():
                        func_name = parts[1].strip()
                        if func_name and current_file:
                            if current_file not in file_functions:
                                file_functions[current_file] = []
                            file_functions[current_file].append(func_name)
                        continue
                        
    except Exception as e:
        print(f"解析 {funs_file} 时出错: {e}")
        import traceback
        traceback.print_exc()
        return {}
    
    return file_functions

def parse_program_output(output_file):
    """解析程序输出的报告文件，提取文件名和函数列表"""
    file_functions = {}
    current_file = None
    in_function_list = False
    
    try:
        with open(output_file, 'r', encoding='utf-8') as f:
            for line in f:
                line_stripped = line.strip()
                line_original = line.rstrip('\r\n')  # 保留行首空白
                
                # 检测是否进入函数定义清单部分（新格式：仅统计有函数体的函数）
                if "按文件分组的函数定义清单" in line_stripped and "仅统计有函数体的函数" in line_stripped:
                    in_function_list = True
                    continue
                
                # 检测是否进入函数声明清单部分（跳过，不统计声明）
                if "按文件分组的函数声明清单" in line_stripped:
                    in_function_list = False
                    continue
                
                # 检测是否进入函数调用关系部分
                if "按文件分组的函数调用关系:" in line_stripped:
                    in_function_list = False
                    continue
                
                if not in_function_list:
                    continue
                
                # 匹配文件行： [文件名] - 包含 N 个函数
                match = re.match(r'\[([^\]]+)\]\s*-\s*包含\s*\d+\s*个函数', line_stripped)
                if match:
                    current_file = match.group(1)
                    # 只提取 .c 和 .h 文件的文件名部分（去掉路径）
                    if '/' in current_file or '\\' in current_file:
                        current_file = os.path.basename(current_file)
                    if current_file not in file_functions:
                        file_functions[current_file] = []
                    continue
                
                # 匹配函数行：  函数名 | 行号: N
                # 格式：  Log_Init                       | 行号: 47
                # 检查行首是否有两个空格，并且包含 | 符号
                if current_file and line_original.startswith('  ') and '|' in line_original:
                    # 提取函数名（在 | 之前的部分）
                    parts = line_original.split('|')
                    if len(parts) >= 1:
                        func_name = parts[0].strip()
                        if func_name:  # 确保函数名不为空
                            file_functions[current_file].append(func_name)
                        
    except Exception as e:
        print(f"解析 {output_file} 时出错: {e}")
        return {}
    
    return file_functions

def normalize_filename(filename):
    """标准化文件名（统一大小写，去掉路径）"""
    # 提取文件名（去掉路径）
    base_name = os.path.basename(filename)
    # 统一转为小写进行比较（但保留原始大小写用于显示）
    return base_name.lower()

def compare_functions(funs_dict, program_dict):
    """对比两个函数字典，找出差异"""
    results = {
        'missing_files': [],  # funs.txt中有但程序输出中没有的文件
        'extra_files': [],    # 程序输出中有但funs.txt中没有的文件
        'common_files': {},   # 共同文件中的函数差异
        'statistics': {
            'funs_total_files': len(funs_dict),
            'program_total_files': len(program_dict),
            'common_files_count': 0,
            'missing_functions_count': 0,
            'extra_functions_count': 0
        }
    }
    
    # 创建文件名映射（忽略大小写）
    funs_files_lower = {normalize_filename(f): f for f in funs_dict.keys()}
    program_files_lower = {normalize_filename(f): f for f in program_dict.keys()}
    
    # 找出缺失的文件
    for funs_file_lower, funs_file in funs_files_lower.items():
        if funs_file_lower not in program_files_lower:
            results['missing_files'].append({
                'file': funs_file,
                'functions': funs_dict[funs_file]
            })
    
    # 找出多余的文件
    for prog_file_lower, prog_file in program_files_lower.items():
        if prog_file_lower not in funs_files_lower:
            results['extra_files'].append({
                'file': prog_file,
                'functions': program_dict[prog_file]
            })
    
    # 对比共同文件
    common_files_lower = set(funs_files_lower.keys()) & set(program_files_lower.keys())
    results['statistics']['common_files_count'] = len(common_files_lower)
    
    for file_lower in common_files_lower:
        funs_file = funs_files_lower[file_lower]
        prog_file = program_files_lower[file_lower]
        
        funs_funcs = set(funs_dict[funs_file])
        prog_funcs = set(program_dict[prog_file])
        
        missing_funcs = funs_funcs - prog_funcs
        extra_funcs = prog_funcs - funs_funcs
        
        if missing_funcs or extra_funcs:
            results['common_files'][funs_file] = {
                'funs_file': funs_file,
                'program_file': prog_file,
                'missing_functions': sorted(missing_funcs),
                'extra_functions': sorted(extra_funcs),
                'funs_count': len(funs_funcs),
                'program_count': len(prog_funcs)
            }
            results['statistics']['missing_functions_count'] += len(missing_funcs)
            results['statistics']['extra_functions_count'] += len(extra_funcs)
    
    return results

def generate_comparison_report(results, output_file):
    """生成对比报告"""
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("="*80 + "\n")
        f.write("函数对比分析报告\n")
        f.write("="*80 + "\n\n")
        
        # 统计信息
        stats = results['statistics']
        f.write("统计信息:\n")
        f.write("-"*80 + "\n")
        f.write(f"funs.txt 中的文件数: {stats['funs_total_files']}\n")
        f.write(f"程序输出中的文件数: {stats['program_total_files']}\n")
        f.write(f"共同文件数: {stats['common_files_count']}\n")
        f.write(f"缺失的函数总数: {stats['missing_functions_count']}\n")
        f.write(f"多出的函数总数: {stats['extra_functions_count']}\n\n")
        
        # 缺失的文件
        if results['missing_files']:
            f.write("\n" + "="*80 + "\n")
            f.write("funs.txt 中有但程序输出中缺失的文件:\n")
            f.write("="*80 + "\n\n")
            for item in results['missing_files']:
                f.write(f"[{item['file']}] - {len(item['functions'])} 个函数\n")
                f.write("-"*80 + "\n")
                for func in item['functions']:
                    f.write(f"  {func}\n")
                f.write("\n")
        
        # 多余的文件
        if results['extra_files']:
            f.write("\n" + "="*80 + "\n")
            f.write("程序输出中有但 funs.txt 中缺失的文件:\n")
            f.write("="*80 + "\n\n")
            for item in results['extra_files']:
                f.write(f"[{item['file']}] - {len(item['functions'])} 个函数\n")
                f.write("-"*80 + "\n")
                for func in item['functions']:
                    f.write(f"  {func}\n")
                f.write("\n")
        
        # 共同文件的函数差异
        if results['common_files']:
            f.write("\n" + "="*80 + "\n")
            f.write("共同文件中的函数差异:\n")
            f.write("="*80 + "\n\n")
            for file_name, diff in sorted(results['common_files'].items()):
                f.write(f"[{file_name}]\n")
                f.write("-"*80 + "\n")
                f.write(f"  funs.txt 中的函数数: {diff['funs_count']}\n")
                f.write(f"  程序输出中的函数数: {diff['program_count']}\n\n")
                
                if diff['missing_functions']:
                    f.write(f"  funs.txt 中有但程序输出中缺失的函数 ({len(diff['missing_functions'])} 个):\n")
                    for func in diff['missing_functions']:
                        f.write(f"    - {func}\n")
                    f.write("\n")
                
                if diff['extra_functions']:
                    f.write(f"  程序输出中有但 funs.txt 中缺失的函数 ({len(diff['extra_functions'])} 个):\n")
                    for func in diff['extra_functions']:
                        f.write(f"    + {func}\n")
                    f.write("\n")
        
        # 总结
        f.write("\n" + "="*80 + "\n")
        f.write("对比完成\n")
        f.write("="*80 + "\n")
    
    print(f"对比报告已生成: {output_file}")

def main():
    parser = argparse.ArgumentParser(description='对比 funs.txt 与程序输出的函数列表')
    parser.add_argument('-f', '--funs', default='examples/funs.txt',
                        help='funs.txt 文件路径 (默认: examples/funs.txt)')
    parser.add_argument('-o', '--output', default='test_output/program_output.txt',
                        help='程序输出的报告文件路径 (默认: test_output/program_output.txt)')
    parser.add_argument('-r', '--report', default='test_output/comparison_report.txt',
                        help='对比报告输出路径 (默认: test_output/comparison_report.txt)')
    
    args = parser.parse_args()
    
    print("正在解析 funs.txt 文件...")
    funs_dict = parse_funs_file(args.funs)
    print(f"从 funs.txt 解析到 {len(funs_dict)} 个文件的函数")
    
    print("正在解析程序输出文件...")
    program_dict = parse_program_output(args.output)
    print(f"从程序输出解析到 {len(program_dict)} 个文件的函数")
    
    print("正在对比函数列表...")
    results = compare_functions(funs_dict, program_dict)
    
    print("正在生成对比报告...")
    # 确保输出目录存在
    report_dir = os.path.dirname(args.report)
    if report_dir and not os.path.exists(report_dir):
        os.makedirs(report_dir)
    
    generate_comparison_report(results, args.report)
    
    # 打印简要统计
    stats = results['statistics']
    print("\n对比完成!")
    print(f"  缺失的文件: {len(results['missing_files'])} 个")
    print(f"  多余的文件: {len(results['extra_files'])} 个")
    print(f"  有差异的共同文件: {len(results['common_files'])} 个")
    print(f"  缺失的函数: {stats['missing_functions_count']} 个")
    print(f"  多出的函数: {stats['extra_functions_count']} 个")

if __name__ == '__main__':
    main()

