import json
from datetime import datetime

class Message:
    def __init__(self, role, content):
        self.role = role
        self.content = content

class Conversation:
    def __init__(self):
        self.messages = []
    
    def add_message(self, role, content):
        message = Message(role, content)
        self.messages.append(message)
    
    def get_messages_for_api(self):
        return [
            {"role": msg.role, "content": msg.content}
            for msg in self.messages
        ]
    
    def clear(self):
        self.messages = []