# Projeto CEtcd - Sistemas Distribuídos (CORBA/TAO)

Este projeto implementa um serviço de diretório distribuído (estilo etcd) utilizando CORBA/TAO. O servidor armazena pares chave-valor e expõe operações remotas de `put`, `get` e `del` via interface IDL.

---

## O que foi implementado

1. **Interface IDL (`CEtcd.idl`):** atributo `id`, métodos `put`/`get`/`del` e exceção `InvalidKey`.
2. **Servant (`CEtcdI.h/.cpp`):** armazenamento em `std::map`, com:
   - **Persistência** — dados salvos em `etcd_data.txt` (leitura no construtor, escrita no destrutor)
   - **Concorrência** — `std::mutex` protegendo o map em cenários multi-thread
   - **Validação** — `put` rejeita chave vazia ou com caracteres de controle (`\t`, `\n`, `\r`)
3. **Servidor (`server.cpp`):** publica o IOR do objeto em arquivo `cetcd.ior` (sem necessidade de Naming Service)
4. **Cliente (`client.cpp`):** CLI interativa com comandos `put`, `get`, `del` e `exit`

---

## Como compilar e rodar

### 1. Gerar stubs do IDL
```bash
tao_idl CEtcd.idl
```

### 2. Compilar
```bash
g++ -std=c++17 -I/opt/ACE_wrappers -I/opt/ACE_wrappers/TAO -I/opt/ACE_wrappers/TAO/orbsvcs server.cpp CEtcdC.cpp CEtcdS.cpp CEtcdI.cpp -L/opt/ACE_wrappers/lib -lTAO -lTAO_AnyTypeCode -lTAO_CosNaming -lTAO_PortableServer -lACE -o servidor_etcd

g++ -std=c++17 -I/opt/ACE_wrappers -I/opt/ACE_wrappers/TAO -I/opt/ACE_wrappers/TAO/orbsvcs client.cpp CEtcdC.cpp -L/opt/ACE_wrappers/lib -lTAO -lTAO_AnyTypeCode -lTAO_CosNaming -lACE -o cliente_etcd
```

> **Nota:** a versão instalada do TAO/ACE requer `-std=c++17`. Compilar com versão anterior causará erro.

### 3. Rodar (2 terminais)

**Terminal 1 — Servidor:**
```bash
./servidor_etcd
```
O servidor cria o arquivo `cetcd.ior` com a referência do objeto.

**Terminal 2 — Cliente** (após o servidor estar no ar):
```bash
./cliente_etcd
```

### 4. Comandos disponíveis no cliente
```
> put <chave> <valor>   # insere ou atualiza
> get <chave>           # busca valor
> del <chave>           # remove
> exit                  # encerra
```

---

Desenvolvido por: Dani Heart Basso, Eduardo Vieira da Silva e Mariana Alves da Silva.
