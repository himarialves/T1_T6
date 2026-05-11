#ifndef CETCDI_H
#define CETCDI_H

#include "CEtcdS.h" // gerado pelo tao_idl — contém a classe base POA_CETcd
#include <map>
#include <mutex>
#include <string>

// Implementação concreta do servant CORBA.
// Herda de POA_CETcd (skeleton gerado pelo tao_idl) e fornece
// a lógica real dos métodos definidos em CEtcd.idl.
class CEtcdI : public virtual POA_CETcd {
public:
    // filepath: arquivo de persistência; padrão "etcd_data.txt" no diretório atual
    CEtcdI(const char* id, const char* filepath = "etcd_data.txt");
    virtual ~CEtcdI(); // salva os dados em disco antes de destruir

    virtual char* id();
    virtual CORBA::Boolean put(const char* key, const char* val);
    virtual char* get(const char* key);
    virtual void del(const char* key);

private:
    void loadFromFile(); // lê pares chave-valor do arquivo na inicialização
    void saveToFile();   // persiste o estado atual no arquivo ao encerrar
    bool isValidKey(const char* key); // rejeita chave vazia ou com caracteres de controle

    std::string m_id;
    std::string m_filepath;
    std::map<std::string, std::string> m_data; // armazenamento em memória
    std::mutex m_mutex; // protege m_data em cenários multi-thread
};

#endif
