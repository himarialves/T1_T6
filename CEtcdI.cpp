#include "CEtcdI.h"
#include <fstream>
#include <iostream>

CEtcdI::CEtcdI(const char* id, const char* filepath) : m_id(id), m_filepath(filepath) {
    loadFromFile(); // restaura estado persistido antes de aceitar requisições
    std::cout << "Servant CEtcd [" << m_id << "] instanciado." << std::endl;
}

CEtcdI::~CEtcdI() {
    saveToFile(); // garante que nenhum dado seja perdido ao encerrar o servidor
}

// Lê o arquivo linha a linha; cada linha tem formato "chave\tvalor".
// Tab como separador evita ambiguidade caso o valor contenha '=', ':', etc.
// Se o arquivo não existir (primeira execução), simplesmente ignora.
void CEtcdI::loadFromFile() {
    std::ifstream file(m_filepath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        auto sep = line.find('\t');
        if (sep == std::string::npos) continue; // linha malformada — ignora
        m_data[line.substr(0, sep)] = line.substr(sep + 1);
    }
    std::cout << "[PERSIST] " << m_data.size() << " entradas carregadas de " << m_filepath << std::endl;
}

// Sobrescreve o arquivo inteiro com o estado atual do map.
// ok para o volume de dados esperado nesse trabalho.
void CEtcdI::saveToFile() {
    std::ofstream file(m_filepath);
    if (!file.is_open()) {
        std::cerr << "[PERSIST] Erro ao salvar " << m_filepath << std::endl;
        return;
    }
    for (const auto& par : m_data) {
        file << par.first << '\t' << par.second << '\n';
    }
    std::cout << "[PERSIST] " << m_data.size() << " entradas salvas em " << m_filepath << std::endl;
}

// Chave inválida se: nula, vazia, ou contém tab/newline (quebraria o formato do arquivo).
bool CEtcdI::isValidKey(const char* key) {
    if (key == nullptr || key[0] == '\0') return false;
    for (const char* c = key; *c != '\0'; ++c) {
        if (*c == '\t' || *c == '\n' || *c == '\r') return false;
    }
    return true;
}

char* CEtcdI::id() {
    return CORBA::string_dup(m_id.c_str());
}

// Retorna true se for nova inserção, false se atualizar valor existente.
CORBA::Boolean CEtcdI::put(const char* key, const char* val) {
    if (!isValidKey(key)) {
        InvalidKey ex;
        ex.message = CORBA::string_dup("Chave invalida: vazia ou com caracteres especiais.");
        throw ex;
    }

    std::lock_guard<std::mutex> lock(m_mutex); // trava para acesso exclusivo ao map
    bool existe = (m_data.find(key) != m_data.end());
    m_data[key] = val;
    std::cout << "[PUT] Chave: " << key << " | Valor: " << val << std::endl;
    return !existe;
}

// CORBA exige que strings retornadas sejam alocadas com string_dup;
// o cliente é responsável por liberar a memória via CORBA::string_free.
char* CEtcdI::get(const char* key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_data.find(key);
    if (it == m_data.end()) {
        std::cerr << "[GET] Erro: Chave " << key << " nao encontrada." << std::endl;
        InvalidKey ex;
        ex.message = CORBA::string_dup("Chave nao encontrada.");
        throw ex;
    }
    return CORBA::string_dup(it->second.c_str());
}

void CEtcdI::del(const char* key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_data.find(key);
    if (it == m_data.end()) {
        std::cerr << "[DEL] Erro: Impossivel remover " << key << "." << std::endl;
        InvalidKey ex;
        ex.message = CORBA::string_dup("Chave inexistente para delecao.");
        throw ex;
    }
    m_data.erase(it);
    std::cout << "[DEL] Chave " << key << " removida com sucesso." << std::endl;
}
