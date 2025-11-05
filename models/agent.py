from config.settings import Config
from models.conversation import Conversation
from utility.helper import APIHelper

class DeepSeekAgent:
    def __init__(self):
        self.config = Config()
        self.conversation = Conversation()
        self.api_helper = APIHelper(self.config)
    
    def initialize(self):
        is_valid, message = self.config.validate()
        if not is_valid:
            return False, message
        return True, "初始化成功"
    
    def process_message(self, user_input):
        self.conversation.add_message("user", user_input)
        
        success, response = self.api_helper.send_chat_request(
            self.conversation.get_messages_for_api()
        )
        
        if success:
            self.conversation.add_message("assistant", response)
            return True, response
        else:
            return False, response
    
    def clear_conversation(self):
        self.conversation.clear()
        return "对话历史已清空"