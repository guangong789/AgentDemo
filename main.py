import readline
import os
import sys

sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from utility.helper import CommandParser
from models.agent import DeepSeekAgent

def main():
    try:
        agent = DeepSeekAgent()
        success, message = agent.initialize()
        
        if not success:
            print(f"错误: {message}")
            return
        print("🤖 DeekSeek Chat 💭")
        while True:
            try:
                user_input = input("👨 User: ").strip()
                if not user_input:
                    continue
                command, args = CommandParser.parse_command(user_input)
                if command:
                    if command in ['/exit']:
                        break
                    elif command in ['/clear']:
                        result = agent.clear_conversation()
                        print(result)
                    elif command in ['/help']:
                        print("/help /clear /exit")
                    else:
                        print("未知命令")
                else:
                    print("🤖 Assistant: ", end='', flush=True)
                    try:
                        success, response = agent.process_message(user_input)
                        if not success and response != "用户中断":
                            print(f"错误: {response}")
                        print()
                    except KeyboardInterrupt:
                        print(" [输入中断]")
                        print()
            
            except KeyboardInterrupt:
                print()
                break
            except EOFError:
                break
    
    except Exception as e:
        print(f"初始化失败: {e}")

if __name__ == "__main__":
    main()