from transformers import AutoTokenizer, AutoModelForCausalLM, TextIteratorStreamer
import torch
from threading import Thread
import PyPDF2
import io
from docx import Document
import pandas as pd

# 配置参数
MODEL_DIR = r"D:/TYUT/LLM/Day10/demo_10/Llama3_test/deepseek-ai/DeepSeek-R1-Distill-Qwen-1___5B/"
MAX_HISTORY = 3  # 保留最近3轮对话历史
GENERATION_CONFIG = {
    "max_new_tokens": 1024,
    "temperature": 0.5,
    "top_p": 0.8,
    "repetition_penalty": 1.2,
    "do_sample": True,
}


# 初始化模型
def load_model():
    try:
        model = AutoModelForCausalLM.from_pretrained(
            MODEL_DIR,
            torch_dtype=torch.float16,
            device_map="auto",
            trust_remote_code=True
        )
        tokenizer = AutoTokenizer.from_pretrained(MODEL_DIR)
        return model, tokenizer
    except Exception as e:
        print(f"模型加载失败: {e}")
        exit(1)


# 构建推理提示
def build_reasoning_prompt(question, history, file_content=None):
    system_prompt = """你是一个专业且严谨的AI助手。请严格按照以下步骤分析和回答问题："""
    if file_content:
        system_prompt += f"\n\n[附加文件内容]\n{file_content}\n\n请结合上述文件内容回答用户问题。"

    messages = [{"role": "system", "content": system_prompt}]
    for h in history[-MAX_HISTORY:]:
        messages.extend([
            {"role": "user", "content": h["question"]},
            {"role": "assistant", "content": h["answer"]}
        ])
    messages.append({"role": "user", "content": question})
    return messages


# 流式生成响应
def generate_response_stream(model, tokenizer, messages):
    try:
        text = tokenizer.apply_chat_template(
            messages,
            tokenize=False,
            add_generation_prompt=True
        )
        inputs = tokenizer(text, return_tensors="pt").to(model.device)

        streamer = TextIteratorStreamer(tokenizer, skip_prompt=True, skip_special_tokens=True)
        generation_kwargs = dict(
            **inputs,
            streamer=streamer,
            **GENERATION_CONFIG
        )

        thread = Thread(target=model.generate, kwargs=generation_kwargs)
        thread.start()

        for new_text in streamer:
            yield new_text
    except Exception as e:
        yield f"[生成错误：{str(e)}]"


def parse_uploaded_file(file_content, filename):
    file_ext = filename.split('.')[-1].lower()

    try:
        if file_ext == 'pdf':
            reader = PyPDF2.PdfReader(io.BytesIO(file_content))
            text = '\n'.join([page.extract_text() for page in reader.pages if page.extract_text() is not None])
            if not text.strip():
                raise ValueError("PDF文件解析失败或内容为空")
        elif file_ext in ['doc', 'docx']:
            doc = Document(io.BytesIO(file_content))
            text = '\n'.join([para.text for para in doc.paragraphs if para.text])
            if not text.strip():
                raise ValueError("Word文件解析失败或内容为空")
        elif file_ext in ['xlsx', 'xls']:
            df = pd.read_excel(io.BytesIO(file_content))
            if df.empty:
                raise ValueError("Excel文件解析失败或内容为空")
            text = df.to_string(index=False)
        else:  # 默认按文本处理
            text = file_content.decode('utf-8')
        return text
    except Exception as e:
        raise ValueError(f"文件解析失败: {str(e)}")