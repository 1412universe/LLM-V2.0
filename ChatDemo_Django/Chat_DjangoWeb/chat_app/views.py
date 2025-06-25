from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.shortcuts import render
from transformers import AutoTokenizer, AutoModelForCausalLM, TextStreamer
import torch

# 配置参数
MODEL_DIR = r"D:/TYUT/LLM/Day10/demo_10/Llama3_test/Qwen/Qwen2___5-1___5B-Instruct/"
MAX_HISTORY = 3  # 保留最近3轮对话历史
GENERATION_CONFIG = {
    "max_new_tokens": 1024,  # 增加生成长度
    "temperature": 0.5,      # 降低随机性
    "top_p": 0.8,            # 限制采样范围
    "repetition_penalty": 1.2,
    "do_sample": True,
}

# 初始化模型
try:
    model = AutoModelForCausalLM.from_pretrained(
        MODEL_DIR,
        torch_dtype=torch.float16,
        device_map="auto",
        trust_remote_code=True
    )
    tokenizer = AutoTokenizer.from_pretrained(MODEL_DIR)
except Exception as e:
    print(f"模型加载失败: {e}")


def build_reasoning_prompt(question, history):
    system_prompt = """你是一个专业且严谨的AI助手。请严格按照以下步骤分析和回答问题：
1. [问题理解]：明确问题的核心要求，分析问题的背景和目标。
2. [关键要素]：列出问题的主要组成部分或关键点。
3. [逻辑推理]：对每个关键要素进行详细分析，提供逻辑推理过程。
4. [综合结论]：基于以上分析，给出清晰、简洁的最终回答。

请确保按照以下格式输出：
[问题理解]
- 分析内容

[关键要素]
- 要素1
- 要素2

[逻辑推理]
- 对要素1的分析
- 对要素2的分析

[综合结论]
- 最终回答

如果问题过于简单或无法分解，请直接在[综合结论]中回答。"""

    messages = [{"role": "system", "content": system_prompt}]

    # 添加历史对话
    for h in history[-MAX_HISTORY:]:
        messages.extend([
            {"role": "user", "content": h["question"]},
            {"role": "assistant", "content": h["answer"]}
        ])

    # 添加当前问题
    messages.append({"role": "user", "content": question})
    return messages


def format_reasoning_output(response):
    sections = {
        "问题理解": "",
        "关键要素": "",
        "逻辑推理": "",
        "综合结论": ""
    }

    current_section = None
    for line in response.split("\n"):
        if line.startswith("["):
            current_section = line.strip("[]")
        elif current_section and current_section in sections:
            sections[current_section] += line + "\n"
    print(sections)
    return sections["综合结论"].strip()


# 生成响应
def generate_response(messages):
    try:
        text = tokenizer.apply_chat_template(
            messages,
            tokenize=False,
            add_generation_prompt=True
        )

        inputs = tokenizer(text, return_tensors="pt").to(model.device)
        streamer = TextStreamer(tokenizer, skip_prompt=True, skip_special_tokens=True)

        output = model.generate(
            **inputs,
            streamer=streamer,
            **GENERATION_CONFIG
        )

        response = tokenizer.decode(output[0][inputs.input_ids.shape[-1]:],
                                    skip_special_tokens=True)
        return response.strip() if response.strip() else "[综合结论]\n抱歉，我暂时无法回答这个问题。"
    except Exception as e:
        print(f"生成错误: {e}")
        return "[综合结论]\n生成回答时遇到问题，请稍后再试。"


from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
import json


@csrf_exempt
def chat_api(request):
    # 对话历史
    history = []
    if request.method == 'POST':
        try:
            data = json.loads(request.body)
            question = data.get('question', '')

            if not question:
                return JsonResponse({'error': '问题不能为空'}, status=400)

            # 构建提示并生成响应
            messages = build_reasoning_prompt(question, history)
            raw_response = generate_response(model, tokenizer, messages)
            final_answer = format_reasoning_output(raw_response)

            # 维护对话历史
            history.append({
                "question": question,
                "answer": final_answer
            })
            history = history[-3:]  # 保持最近3轮对话

            return JsonResponse({'answer': final_answer})
        except Exception as e:
            return JsonResponse({'error': str(e)}, status=500)
    else:
        return JsonResponse({'error': '仅支持POST请求'}, status=405)
#
# @csrf_exempt
# def chat_api(request):
#     if request.method == 'POST':
#         data = request.POST
#         question = data.get('question')
#         history = eval(data.get('history', '[]'))
#
#         messages = build_reasoning_prompt(question, history)
#         raw_response = generate_response(messages)
#         final_answer = format_reasoning_output(raw_response)
#
#         history.append({
#             "question": question,
#             "answer": final_answer
#         })
#         history = history[-MAX_HISTORY:]
#
#         return JsonResponse({
#             'answer': final_answer,
#             'history': history
#         })
#     return JsonResponse({'error': '仅支持 POST 请求'}, status=405)

@csrf_exempt
def chat_page(request):
    return render(request, 'chat.html')

@csrf_exempt
def index(request):
    return render(request, 'chat.html')