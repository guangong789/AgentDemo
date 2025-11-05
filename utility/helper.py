import requests
import json

class APIHelper:
    def __init__(self, config):
        self.config = config
        self.session = requests.Session()
        self.session.headers.update({
            'Authorization': f'Bearer {config.api_key}',
            'Content-Type': 'application/json'
        })
    
    def send_chat_request(self, messages):
        system_message = {"role": "system", "content": "请用中文回答所有问题,除非用户明确指定语言"}
        all_messages = [system_message] + messages
        
        payload = {
            "model": "deepseek-chat",
            "messages": all_messages,  # 使用包含system消息的列表
            "temperature": 0.7,
            "max_tokens": 4096,
            "stream": True
        }
        
        try:
            return self._send_stream_request(payload)
        except requests.exceptions.RequestException as e:
            return False, f"请求错误: {e}"
        except Exception as e:
            return False, f"未知错误: {e}"
    
    def _send_stream_request(self, payload):
        response = self.session.post(
            f"{self.config.base_url}/chat/completions",
            json=payload,
            timeout=30,
            stream=True
        )
        response.raise_for_status()
        
        full_response = ""
        try:
            for line in response.iter_lines():
                if line:
                    line = line.decode('utf-8')
                    if line.startswith('data: '):
                        data = line[6:]
                        if data == '[DONE]':
                            break
                        try:
                            json_data = json.loads(data)
                            if 'choices' in json_data and len(json_data['choices']) > 0:
                                delta = json_data['choices'][0].get('delta', {})
                                if 'content' in delta:
                                    content = delta['content']
                                    print(content, end='', flush=True)
                                    full_response += content
                        except json.JSONDecodeError:
                            continue
            print()
            return True, full_response
        except KeyboardInterrupt:
            print(" [已中断]")
            return False, "用户中断"

class CommandParser:
    @staticmethod
    def parse_command(user_input):
        if not user_input.startswith('/'):
            return None, []
        
        parts = user_input.split()
        command = parts[0].lower()
        args = parts[1:] if len(parts) > 1 else []
        
        return command, args