from django.http import StreamingHttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from .utils import load_model, build_reasoning_prompt, generate_response_stream, parse_uploaded_file
import json

# 加载模型
model, tokenizer = load_model()

@csrf_exempt
def chat_api(request):
    if request.method == 'POST':
        try:
            # 处理文件上传
            file_content = None
            if 'file' in request.FILES:
                uploaded_file = request.FILES['file']
                file_content = parse_uploaded_file(uploaded_file.read(), uploaded_file.name)

            data = json.loads(request.POST.get('data', '{}'))
            question = data.get('question', '')
            history = data.get('history', [])

            # 解析请求数据
            data = json.loads(request.body)
            question = data.get('question', '')
            history = data.get('history', [])
            file_content = data.get('file_content', None)

            if not question:
                return JsonResponse({'error': '问题不能为空'}, status=400)

            # 构建提示
            messages = build_reasoning_prompt(question, history, file_content)

            # 流式生成响应
            def event_stream():
                try:
                    for chunk in generate_response_stream(model, tokenizer, messages):
                        yield json.dumps({"answer": chunk, "status": "streaming"}) + "\n"
                    yield json.dumps({"status": "completed"}) + "\n"  # 标记流式结束
                except Exception as e:
                    yield json.dumps({"error": str(e), "status": "error"}) + "\n"

            return StreamingHttpResponse(event_stream(), content_type='application/x-ndjson')
        except Exception as e:
            return JsonResponse({'error': str(e), "status": "error"}, status=500)
    return JsonResponse({'error': '仅支持POST请求', "status": "error"}, status=405)