#include "CEtcdI.h"
#include <orbsvcs/CosNamingC.h>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        // 1. Inicializa o ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // 2. Obtem o RootPOA e ativa o Manager
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var mgr = poa->the_POAManager();
        mgr->activate();

        // 3. Instancia o nosso Servant CEtcdI
        CEtcdI* etcd_impl = new CEtcdI("ServidorPrincipal");
        PortableServer::ObjectId_var id = poa->activate_object(etcd_impl);
        CORBA::Object_var etcd_obj = poa->id_to_reference(id.in());

        // 4. Conecta no Servidor de Nomes e publica
        CORBA::Object_var ns_obj = orb->resolve_initial_references("NameService");
        CosNaming::NamingContext_var nc = CosNaming::NamingContext::_narrow(ns_obj.in());

        CosNaming::Name name;
        name.length(1);
        name[0].id = CORBA::string_dup("ObjetoCEtcd");
        name[0].kind = CORBA::string_dup("");

        nc->rebind(name, etcd_obj.in());
        std::cout << "Servidor CEtcd publicado e aguardando chamadas..." << std::endl;

        // 5. Roda o loop principal do servidor
        orb->run();

        // Limpeza (só roda se o servidor for desligado)
        poa->destroy(1, 1);
        orb->destroy();
    } catch(const CORBA::Exception& ex) {
        std::cerr << "Erro CORBA: " << ex._info().c_str() << std::endl;
        return 1;
    }
    return 0;
}
