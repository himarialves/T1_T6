# Projeto CEtcd - Sistemas Distribuídos (CORBA/TAO)

Este projeto implementa um serviço de diretório distribuído (estilo etcd) utilizando CORBA. O servidor armazena pares chave-valor em memória e permite operações remotas via NameService.

## O que já foi feito
1. *Interface IDL (CEtcd.idl):* Definida com suporte a strings e exceção InvalidKey.
2. *Implementação do Servant (CEtcdI.h/.cpp):* Lógica de armazenamento usando std::map<std::string, std::string>.
3. *Servidor (server.cpp):* Configurado para registrar o objeto no CosNaming (NameService).
4. *Cliente de Teste (client.cpp):* Implementado para validar PUT, GET, DEL e tratamento de exceções.

## Atualizacao das features implementadas: 

# : 1
Feature: Persistência
Arquivos alterados: CEtcdI.h/cpp — loadFromFile() no construtor,
saveToFile()
no destrutor, formato chave\tvalor por linha

#  2
Feature: CLI interativa
Arquivos alterados: client.cpp — loop >  com put, get, del, exit

# 3
Feature: Mutex
Arquivos alterados: CEtcdI.h/cpp — std::mutex m_mutex + std::lock_guard em
Arquivos alterados: CEtcdI.h/cpp — std::mutex m_mutex + std::lock_guard em todos os métodos

# : 4
Feature: Validação no put
Arquivos alterados: CEtcdI.cpp — rejeita chave vazia ou com \t/\n/\r, lança InvalidKey

## Como rodar? 

**Passo 1 — Gerar stubs do IDL:**
```bash
cd T1_vf
tao_idl CEtcd.idl
```

**Passo 2 — Compilar o servidor:**
```bash
g++ -std=c++11 -I/opt/ACE_wrappers -I/opt/ACE_wrappers/TAO -I/opt/ACE_wrappers/TAO/orbsvcs \
    server.cpp CEtcdC.cpp CEtcdS.cpp CEtcdI.cpp \
    -L/opt/ACE_wrappers/lib -lTAO -lTAO_AnyTypeCode -lTAO_CosNaming -lTAO_PortableServer -lACE \
    -o servidor_etcd
```

**Passo 3 — Compilar o cliente**
```bash
g++ -std=c++11 -I/opt/ACE_wrappers -I/opt/ACE_wrappers/TAO -I/opt/ACE_wrappers/TAO/orbsvcs \
    client.cpp CEtcdC.cpp \
    -L/opt/ACE_wrappers/lib -lTAO -lTAO_AnyTypeCode -lTAO_CosNaming -lACE \
    -o cliente_etcd
```
  
## Para rodar:

Rodar **nessa** ordem:

```bash
# 1. CORBA Naming Service
/opt/ACE_wrappers/bin/Naming_Service -m 1 &

# 2. Server (registers "ObjetoCEtcd" in NameService)
./servidor_etcd &

# 3. Client (looks up "ObjetoCEtcd" and runs tests)
./cliente_etcd
```
---
Desenvolvido por: Dani Heart Basso, Eduardo Vieira e Mariana Alves da Silva. 
