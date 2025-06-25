from transformers import AutoTokenizer, AutoModelForCausalLM, TextStreamer
import torch

# 配置参数
MODEL_DIR = r"/root/autodl-tmp/demo_11/Llama-3___2-1B-Instruct-lora/"
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
def build_reasoning_prompt(question, history):
    system_prompt = """你是一个专业且严谨的AI助手。请严格按照以下步骤分析和回答问题：..."""
    messages = [{"role": "system", "content": system_prompt}]
    for h in history[-MAX_HISTORY:]:
        messages.extend([
            {"role": "user", "content": h["question"]},
            {"role": "assistant", "content": h["answer"]}
        ])
    messages.append({"role": "user", "content": question})
    return messages

# 生成响应
def generate_response(model, tokenizer, messages):
    try:
        text = tokenizer.apply_chat_template(
            messages,
            tokenize=False,
            add_generation_prompt=True
        )
        print(f"生成的输入文本: {text}")  # 调试信息
        inputs = tokenizer(text, return_tensors="pt").to(model.device)
        streamer = TextStreamer(tokenizer, skip_prompt=True, skip_special_tokens=True)
        output = model.generate(
            **inputs,
            streamer=streamer,
            **GENERATION_CONFIG
        )
        print(f"模型生成的输出: {output}")  # 调试信息
        response = tokenizer.decode(output[0][inputs.input_ids.shape[-1]:], skip_special_tokens=True)
        print(f"解码后的响应: {response}")  # 调试信息
        return response.strip() if response.strip() else "[综合结论]\n抱歉，我暂时无法回答这个问题。"
    except Exception as e:
        print(f"生成错误: {e}")
        return "[综合结论]\n生成回答时遇到问题，请稍后再试。"

# 格式化输出
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
    result = sections["综合结论"].strip()
    if not result:
        print("格式化后的综合结论为空，返回原始响应。")
        return response.strip() if response.strip() else "[综合结论]\n抱歉，我暂时无法回答这个问题。"
    return result

# # 示例使用
# model, tokenizer = load_model()
# question = "这是一个测试问题。"
# history = []
# messages = build_reasoning_prompt(question, history)
# response = generate_response(model, tokenizer, messages)
# answer = format_reasoning_output(response)
# print(f"最终回答: {answer}")