#include "CEtcdC.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Lê o IOR do arquivo gerado pelo servidor
#define IOR_FILE "cetcd.ior"

int main(int argc, char* argv[]) {
    try {
        // Inicializa o ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // Lê o IOR do arquivo
        std::ifstream ior_file(IOR_FILE);
        if (!ior_file) {
            std::cerr << "Erro: arquivo " << IOR_FILE << " não encontrado." << std::endl;
            std::cerr << "Certifique-se de que o servidor está rodando." << std::endl;
            return 1;
        }
        std::string ior;
        std::getline(ior_file, ior);
        ior_file.close();

        // Converte a string IOR para referência de objeto e faz narrow para CETcd
        CORBA::Object_var obj = orb->string_to_object(ior.c_str());
        CETcd_var etcd = CETcd::_narrow(obj.in());

        if (CORBA::is_nil(etcd.in())) {
            std::cerr << "Erro: não foi possível conectar ao objeto CEtcd." << std::endl;
            return 1;
        }

        std::cout << "Conectado ao CEtcd [" << etcd->id() << "]" << std::endl;
        std::cout << "Comandos: put <chave> <valor> | get <chave> | del <chave> | exit" << std::endl;

        // Loop interativo: lê um comando por linha até EOF ou "exit"
        std::string line;
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;

            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;

            if (cmd == "exit" || cmd == "quit") break;

            if (cmd == "put") {
                std::string key, val;
                iss >> key;
                std::getline(iss >> std::ws, val); // resto da linha é o valor
                if (key.empty()) { std::cout << "Uso: put <chave> <valor>" << std::endl; continue; }
                try {
                    bool isNew = etcd->put(key.c_str(), val.c_str());
                    std::cout << (isNew ? "Inserido." : "Atualizado.") << std::endl;
                } catch (const InvalidKey& e) {
                    std::cout << "Erro: " << e.message << std::endl;
                }
            } else if (cmd == "get") {
                std::string key;
                iss >> key;
                if (key.empty()) { std::cout << "Uso: get <chave>" << std::endl; continue; }
                try {
                    // String_var gerencia automaticamente a memória alocada pelo CORBA
                    CORBA::String_var result = etcd->get(key.c_str());
                    std::cout << result.in() << std::endl;
                } catch (const InvalidKey& e) {
                    std::cout << "Erro: " << e.message << std::endl;
                }
            } else if (cmd == "del") {
                std::string key;
                iss >> key;
                if (key.empty()) { std::cout << "Uso: del <chave>" << std::endl; continue; }
                try {
                    etcd->del(key.c_str());
                    std::cout << "Removido." << std::endl;
                } catch (const InvalidKey& e) {
                    std::cout << "Erro: " << e.message << std::endl;
                }
            } else if (!cmd.empty()) {
                std::cout << "Comando desconhecido: " << cmd << std::endl;
            }
        }

        orb->destroy();
    } catch (const CORBA::Exception& ex) {
        std::cerr << "Erro CORBA: " << ex._info().c_str() << std::endl;
        return 1;
    }
    return 0;
}
