#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <json/json.h>
#include <sstream>

class OpenAIChatbot {
private:
    std::string apiKey;
    std::string apiUrl = "https://api.openai.com/v1/chat/completions";
    std::vector<Json::Value> conversationHistory;
    

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    

    std::string makeAPIRequest(const std::string& jsonPayload) {
        CURL* curl;
        CURLcode res;
        std::string response;
        
        curl = curl_easy_init();
        if(curl) {
            
            struct curl_slist* headers = nullptr;
            std::string authHeader = "Authorization: Bearer " + apiKey;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, authHeader.c_str());
            

            curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            
    res = curl_easy_perform(curl);
            
   curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        
        return response;
    }
    

    std::string createJSONPayload(const std::string& userMessage) {
        Json::Value root;
        Json::Value messages(Json::arrayValue);
        

        for(const auto& msg : conversationHistory) {
            messages.append(msg);
        }
        
        
        Json::Value userMsg;
        userMsg["role"] = "user";
        userMsg["content"] = userMessage;
        messages.append(userMsg);
        
        root["model"] = "gpt-4o-mini";
        root["messages"] = messages;
        root["max_tokens"] = 150;
        root["temperature"] = 0.7;
        
        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, root);
    }
    
    std::string parseResponse(const std::string& response) {
        Json::Reader reader;
        Json::Value root;
        
        if(reader.parse(response, root)) {
            if(root.isMember("choices") && root["choices"].isArray() && root["choices"].size() > 0) {
                Json::Value choice = root["choices"][0];
                if(choice.isMember("message") && choice["message"].isMember("content")) {
                    return choice["message"]["content"].asString();
                }
            }
            
            
            if(root.isMember("error")) {
                return "Error: " + root["error"]["message"].asString();
            }
        }
        
        return "Failed to parse response";
    }

public:
    OpenAIChatbot(const std::string& key) : apiKey(key) {

        Json::Value systemMsg;
        systemMsg["role"] = "system";
        systemMsg["content"] = "You are a helpful assistant.";
        conversationHistory.push_back(systemMsg);
    }
    
    std::string chat(const std::string& userMessage) {
      
        std::string jsonPayload = createJSONPayload(userMessage);
        
        
        std::string response = makeAPIRequest(jsonPayload);
        
        
        std::string assistantReply = parseResponse(response);
        
        
        Json::Value userMsg;
        userMsg["role"] = "user";
        userMsg["content"] = userMessage;
        conversationHistory.push_back(userMsg);
        
        Json::Value assistantMsg;
        assistantMsg["role"] = "assistant";
        assistantMsg["content"] = assistantReply;
        conversationHistory.push_back(assistantMsg);
        
        return assistantReply;
    }
    
    void clearHistory() {
        conversationHistory.clear();
        Json::Value systemMsg;
        systemMsg["role"] = "system";
        systemMsg["content"] = "You are a helpful assistant.";
        conversationHistory.push_back(systemMsg);
    }
    
    void setSystemMessage(const std::string& systemMessage) {
        if(!conversationHistory.empty()) {
            conversationHistory[0]["content"] = systemMessage;
        }
    }
};

int main() {
    std::cout << "OpenAI Chatbot in C++" << std::endl;
    std::cout << "=====================" << std::endl;
    
    
    std::string apiKey;
    std::cout << "Enter your OpenAI API key: ";
    std::getline(std::cin, apiKey);
    
    if(apiKey.empty()) {
        std::cout << "API key is required!" << std::endl;
        return 1;
    }
    

    OpenAIChatbot chatbot(apiKey);
    
    std::cout << "\nChatbot initialized! Type 'quit' to exit, 'clear' to clear history." << std::endl;
    std::cout << "You can also type 'system <message>' to set a system message." << std::endl;
    std::cout << "\nStart chatting:" << std::endl;
    
    std::string userInput;
    while(true) {
        std::cout << "\nYou: ";
        std::getline(std::cin, userInput);
        
        if(userInput == "quit") {
            break;
        }
        
        if(userInput == "clear") {
            chatbot.clearHistory();
            std::cout << "Conversation history cleared!" << std::endl;
            continue;
        }
        
        if(userInput.substr(0, 7) == "system ") {
            std::string systemMsg = userInput.substr(7);
            chatbot.setSystemMessage(systemMsg);
            std::cout << "System message set!" << std::endl;
            continue;
        }
        
        if(userInput.empty()) {
            continue;
        }
        
        std::cout << "Assistant: ";
        std::string response = chatbot.chat(userInput);
        std::cout << response << std::endl;
    }
    
    std::cout << "Goodbye!" << std::endl;
    return 0;
}