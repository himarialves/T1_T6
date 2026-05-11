#include "CEtcdI.h"
#include <fstream>
#include <iostream>

// Sem Naming Service disponível, o servidor escreve o IOR em arquivo.
// O cliente lê esse arquivo para localizar o objeto remoto.
#define IOR_FILE "cetcd.ior"

int main(int argc, char* argv[]) {
    try {
        // Inicializa o ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // Obtém o RootPOA e ativa o Manager
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var mgr = poa->the_POAManager();
        mgr->activate();

        // Instancia o servant
        CEtcdI* etcd_impl = new CEtcdI("ServidorPrincipal");
        PortableServer::ObjectId_var id = poa->activate_object(etcd_impl);
        CORBA::Object_var etcd_obj = poa->id_to_reference(id.in());

        // Serializa a referência do objeto para string e salva em arquivo
        CORBA::String_var ior = orb->object_to_string(etcd_obj.in());
        std::ofstream ior_file(IOR_FILE);
        if (!ior_file) {
            std::cerr << "Erro ao criar " << IOR_FILE << std::endl;
            return 1;
        }
        ior_file << ior.in();
        ior_file.close();

        std::cout << "IOR salvo em " << IOR_FILE << std::endl;
        std::cout << "Servidor CEtcd aguardando chamadas..." << std::endl;

        // Roda o loop principal do servidor
        orb->run();

        poa->destroy(1, 1);
        orb->destroy();
    } catch (const CORBA::Exception& ex) {
        std::cerr << "Erro CORBA: " << ex._info().c_str() << std::endl;
        return 1;
    }
    return 0;
}
