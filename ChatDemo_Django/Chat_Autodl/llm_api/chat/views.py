from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from .utils import load_model, build_reasoning_prompt, generate_response, format_reasoning_output
import json

# 加载模型
model, tokenizer = load_model()

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