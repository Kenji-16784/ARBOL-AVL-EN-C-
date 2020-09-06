#include <fstream>
#include <iostream>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>

//PALABRA RESERVADA DEL SISTEMAS PARA USAR "COUT" Y "CIN" EN E/S
using namespace std;

    //DECLARACION DE LA CLASE AVL
    class AVL;

    //VECTORES QUE CONTIENEN RUTAS
    char nomArchivo[]={"BD.txt"};

/*-----------------------------------------------------------------------------------------------------*/
    //PROTOTIPO DE LA ESTRUCTURA ENCARGADA DE EXTRACCION DE DATOS EN LAS FUNCIONES
    struct DATO{
        string codigo;
        string carnet;
        string nombre;
        string sexo;
    };

/*-----------------------------------------------------------------------------------------------------*/
    //ATRIBUTOS DE LA CLASE NODO
    class Nodo {
    public:

    DATO dato;
    int FE;
    Nodo *izquierdo;
    Nodo *derecho;
    Nodo *padre;
    Nodo(DATO dat, Nodo *pad, Nodo *izq, Nodo *der);
    friend class AVL;
};

/*-----------------------------------------------------------------------------------------------------*/
    //CONSTRUCTOR DE LA CLASE NODO
    Nodo::Nodo(DATO dat, Nodo *pad=NULL, Nodo *izq=NULL, Nodo * der=NULL){
        dato = dat;
        FE = 0;
        padre = pad;
        izquierdo = izq;
        derecho = der;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //ATRIBUTOS Y FUNCIONES PUBLICAS Y PRIVADAS DE LA CLASE AVL
    class AVL {
    private:
    enum {IZQUIERDO, DERECHO};

    Nodo *raiz;
    Nodo *actual;
    int contador;
    int altura;

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCIONES PUBLICAS DE LA CLASE AVL
    public:

    AVL() : raiz(NULL), actual(NULL) {}
    ~AVL() { Podar(raiz); }
    void Insertar(DATO);
    void Borrar(string);
    bool Buscar(string);
    bool Vacio(Nodo *r) { return r==NULL; }
    bool EsHoja(Nodo *r) { return !r->derecho && !r->izquierdo; }
    const int NumeroNodos();
    const int AlturaArbol();
    int Altura(string dat);
    void Raiz() { actual = raiz; }
    void InOrden(void (*func)(DATO&, int) , Nodo *nodo=NULL, bool r=true);
    void PreOrden(void (*func)(DATO&, int) , Nodo *nodo=NULL, bool r=true);
    void PostOrden(void (*func)(DATO&, int) , Nodo *nodo=NULL, bool r=true);

    void cargarArbol_Codigo(class AVL&);
    void cargarArbol_Carnet(class AVL&);
    void cargarArbol_Nombre(class AVL&);
    string* split(string);
    void menu(class AVL&,class AVL&,class AVL&);
    void registrar(class AVL&);
    void almacenar(string);
    DATO buscarRegistro(string);
    void buscar(class AVL&);
    void eliminar(class AVL&);
    void eliminarEnArchivo(string);

/*-----------------------------------------------------------------------------------------------------*/
  //FUNCIONES PRIVADAS DE LA CLASE AVL PARA EL EQUILIBRIO DEL ARBOL
  private:
   //FUNCIONES DE EQUILIBRADO
   void Equilibrar(Nodo *nodo, int, bool);
   void RSI(Nodo* nodo);
   void RSD(Nodo* nodo);
   void RDI(Nodo* nodo);
   void RDD(Nodo* nodo);
   //FUNCIONES AUXILIARES
   void Podar(Nodo* &);
   void auxContador(Nodo*);
   void auxAltura(Nodo*, int);
};

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION QUE BORRA TODOS LOS NODOS
    void AVL::Podar(Nodo* &nodo){

   //ALGORTIMO RECURSIVO QUE RECORRE EN POSTORDEN
    if(nodo) {
      Podar(nodo->izquierdo);
      Podar(nodo->derecho);
      delete nodo;
      nodo = NULL;
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION QUE INSERTA LA ESTRUCTURA EN EL NODO
    void AVL::Insertar(DATO dat){
        Nodo *padre = NULL;
        actual = raiz;

        while(!Vacio(actual) && dat.codigo != actual->dato.codigo) {
        padre = actual;
        if(dat.codigo > actual->dato.codigo) actual = actual->derecho;
        else if(dat.codigo < actual->dato.codigo) actual = actual->izquierdo;
    }

    if(!Vacio(actual)) return;
    if(Vacio(padre)) raiz = new Nodo(dat);
    else if(dat.codigo < padre->dato.codigo) {
      padre->izquierdo = new Nodo(dat, padre);
      Equilibrar(padre, IZQUIERDO, true);
        }
    else if(dat.codigo > padre->dato.codigo) {
      padre->derecho = new Nodo(dat, padre);
      Equilibrar(padre, DERECHO, true);
    }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION QUE EQUILIBRA EL ARBOL A PARTIR DE DEL NODO NUEVO
    void AVL::Equilibrar(Nodo *nodo, int rama, bool nuevo){
    bool salir = false;

    //RECORRE EL CAMINO INVERSO ACTUALIZANDO VALORES DE FE
    while(nodo && !salir) {
      if(nuevo)

        //DEPENDE DE SI AÑADIMOS
         if(rama == IZQUIERDO) nodo->FE--;
         else                  nodo->FE++;

        //DEPENDE DE SI BORRAMOS
      else
         if(rama == IZQUIERDO) nodo->FE++;
         else                  nodo->FE--;

        //LA ALTURA DE LAS RAMAS QUE EN NODO NO HA VARIADO ENTONCES SALIR
      if(nodo->FE == 0) salir = true;

        //ROTAR A DERECHAS Y SALIR
      else if(nodo->FE == -2) {

            //ROTACION DOBLE
         if(nodo->izquierdo->FE == 1) RDD(nodo);

            //ROTACION SIMPLE
         else RSD(nodo);
         salir = true;
      }

      //ROTAR A IZQUIERDAS Y SALIR
      else if(nodo->FE == 2) {

            //ROTACION DOBLE
         if(nodo->derecho->FE == -1) RDI(nodo);

            //ROTACION SIMPLE
         else RSI(nodo);
         salir = true;
      }
      if(nodo->padre)
         if(nodo->padre->derecho == nodo) rama = DERECHO; else rama = IZQUIERDO;

         //CALCULAR FE, SIGUIENTE NODO DEL CAMINO
      nodo = nodo->padre;
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA LAS ROTACIONES DOBLES DERECHAS
    void AVL::RDD(Nodo* nodo){

    Nodo *Padre = nodo->padre;
    Nodo *P = nodo;
    Nodo *Q = P->izquierdo;
    Nodo *R = Q->derecho;
    Nodo *B = R->izquierdo;
    Nodo *C = R->derecho;

    if(Padre)
     if(Padre->derecho == nodo) Padre->derecho = R;
     else Padre->izquierdo = R;
    else raiz = R;

    //RECONSTRUIMOS EL ARBOL
    Q->derecho = B;
    P->izquierdo = C;
    R->izquierdo = Q;
    R->derecho = P;

    //REASIGNAMOS PADRES
    R->padre = Padre;
    P->padre = Q->padre = R;
    if(B) B->padre = Q;
    if(C) C->padre = P;

    //AJUSTAMOS VALORES A FE
    switch(R->FE) {
        case -1: Q->FE = 0; P->FE = 1; break;
        case 0:  Q->FE = 0; P->FE = 0; break;
        case 1:  Q->FE = -1; P->FE = 0; break;
    }
    R->FE = 0;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //ROTACIONES DOBLE A IZQUIERDAS
    void AVL::RDI(Nodo* nodo){

    Nodo *Padre = nodo->padre;
    Nodo *P = nodo;
    Nodo *Q = P->derecho;
    Nodo *R = Q->izquierdo;
    Nodo *B = R->izquierdo;
    Nodo *C = R->derecho;

    if(Padre)
     if(Padre->derecho == nodo) Padre->derecho = R;
     else Padre->izquierdo = R;
    else raiz = R;

    //RECONSTRUIMOS EL ARBOL
    P->derecho = B;
    Q->izquierdo = C;
    R->izquierdo = P;
    R->derecho = Q;

    //REASIGNAMOS PADRES
    R->padre = Padre;
    P->padre = Q->padre = R;
    if(B) B->padre = P;
    if(C) C->padre = Q;

    //AJUSTAMOS VALORES DE FE
    switch(R->FE) {
        case -1: P->FE = 0; Q->FE = 1; break;
        case 0:  P->FE = 0; Q->FE = 0; break;
        case 1:  P->FE = -1; Q->FE = 0; break;
    }
    R->FE = 0;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //ROTACION SIMPLE A DERECHAS
    void AVL::RSD(Nodo* nodo){

    Nodo *Padre = nodo->padre;
    Nodo *P = nodo;
    Nodo *Q = P->izquierdo;
    Nodo *B = Q->derecho;

    if(Padre)
     if(Padre->derecho == P) Padre->derecho = Q;
     else Padre->izquierdo = Q;
    else raiz = Q;

    //RECONSTRUIMOS EL ARBOL
    P->izquierdo = B;
    Q->derecho = P;

    //REASIGNAMOS PADRES
    P->padre = Q;
    if(B) B->padre = P;
    Q->padre = Padre;

    //AJUSTAMOS VALORES DE FE
    P->FE = 0;
    Q->FE = 0;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //ROTACION SIMPLE A IZQUIERDAS
    void AVL::RSI(Nodo* nodo){

    Nodo *Padre = nodo->padre;
    Nodo *P = nodo;
    Nodo *Q = P->derecho;
    Nodo *B = Q->izquierdo;

    if(Padre)
        if(Padre->derecho == P) Padre->derecho = Q;
        else Padre->izquierdo = Q;
    else raiz = Q;

    //RECONSTRUIMOS EL ARBOL
    P->derecho = B;
    Q->izquierdo = P;

    //REASIGNAMOS PADRES
    P->padre = Q;
    if(B) B->padre = P;
    Q->padre = Padre;

    //AJUSTAMOS VALORES DE FE
    P->FE = 0;
    Q->FE = 0;
    }

/*-----------------------------------------------------------------------------------------------------*/
     //ELIMINAR UN ELEMENTO DE UN ARBOL
    void AVL::Borrar(string dat){
    Nodo *padre = NULL;
    Nodo *nodo;
    DATO aux;

    actual = raiz;

    //MIESTRAS SEA POSIBLE QUE EL VALOR ESTE EN EL ARBOL
    while(!Vacio(actual)) {

            //SI EL VALOR ESTA EN EL NODO ACTUAL
      if(dat == actual->dato.codigo) {

          //Y SI ADEMAS ES UN NODO HOJA LO BORRAMOS
         if(EsHoja(actual)) {

             //Y SI TIENE PADRE NO ES EL NODO RAIZ
            if(padre)

               //ENTONCES ANULAMOS EL PUNTERO QUE LE HACE REFERENCIA
               if(padre->derecho == actual) padre->derecho = NULL;
               else if(padre->izquierdo == actual) padre->izquierdo = NULL;

                //BORRAMOS EL NODO ACTUAL
            delete actual;
            actual = NULL;

            /*EL NODO PADRE DEL ACTUAL PUEDE SER AHORA UN NODO HOJA, ENTONCES FE = 0
            PERO DEBEMOS SEGUIR EL CAMINO A PARTIR DE SU PADRE EN CASO DE SI EXISTE*/
            if((padre->derecho == actual && padre->FE == 1) ||
               (padre->izquierdo == actual && padre->FE == -1)) {
               padre->FE = 0;
               actual = padre;
               padre = actual->padre;
            }
            if(padre)
               if(padre->derecho == actual) Equilibrar(padre, DERECHO, false);
               else                         Equilibrar(padre, IZQUIERDO, false);
            return;
         }

         //SI EL VALOR ESTA EN EL NODO ACTUAL PERO NO ES HOJA, BUSCAR NODO
         else {
            padre = actual;

            //BUSCAR NODO MAS IZQUIERDO DE RAMA DERECHA
            if(actual->derecho) {
               nodo = actual->derecho;
               while(nodo->izquierdo) {
                  padre = nodo;
                  nodo = nodo->izquierdo;
               }
            }

            //SINO BUSCAR NODO MAS DERECHO DE RAMA IZQUIERDA
            else {
               nodo = actual->izquierdo;
               while(nodo->derecho) {
                  padre = nodo;
                  nodo = nodo->derecho;
               }
            }

            /*INTERCAMBIAR VALORES DE NO A BORRAR UN NODO ENCONTRADO Y CONTINUAR
            CERRRANDO EL BUCLE, EL NODO ENCONTRADO NO TIENE PORQUE SER UN NO HOJA
            CERRANDO EL BUCLE NOS ASEGURAMOS DE QUE SOLO SE ELIMIEN NODOS HOJA*/
            aux = actual->dato;
            actual->dato = nodo->dato;
            nodo->dato = aux;
            actual = nodo;
         }
      }
      //TODAVIA NO HEMOS ENCONTRADO EL VALOR Y SEGUIMOS BUSCANDO
      else {
         padre = actual;
         if(dat > actual->dato.codigo) actual = actual->derecho;
         else if(dat < actual->dato.codigo) actual = actual->izquierdo;
        }
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA RECORRER EL ARBOL EN INORDEN
    void AVL::InOrden(void (*func)(DATO&, int) , Nodo *nodo, bool r){
    if(r) nodo = raiz;
    if(nodo->izquierdo) InOrden(func, nodo->izquierdo, false);
    func(nodo->dato, nodo->FE);
    if(nodo->derecho) InOrden(func, nodo->derecho, false);
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA RECORRER EL ARBOL EN PREORDEN
    void AVL::PreOrden(void (*func)(DATO&, int), Nodo *nodo, bool r){
    if(r) nodo = raiz;
    func(nodo->dato, nodo->FE);
    if(nodo->izquierdo) PreOrden(func, nodo->izquierdo, false);
    if(nodo->derecho) PreOrden(func, nodo->derecho, false);
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA RECORRER EL ARBOL EN POSTORDEN
    void AVL::PostOrden(void (*func)(DATO&, int), Nodo *nodo, bool r){
    if(r) nodo = raiz;
    if(nodo->izquierdo) PostOrden(func, nodo->izquierdo, false);
    if(nodo->derecho) PostOrden(func, nodo->derecho, false);
    func(nodo->dato, nodo->FE);
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA BUSCAR UN VALOR EN EL ARBOL
    bool AVL::Buscar(string dat){
    actual = raiz;

    //MIENTRAS PUEDE APARECER, YA QUE QUEDAN NODOS POR MIRAR
    while(!Vacio(actual)) {
        if(dat == actual->dato.codigo) return true; // dato encontrado
        else if(dat > actual->dato.codigo) actual = actual->derecho; // Seguir
        else if(dat < actual->dato.codigo) actual = actual->izquierdo;
    }

    //SI NO LO ENCUENTRA ES PORQUE NO ESTA EN EL ARBOL
    return false;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA CALCULAR LA ALTURA DEL NODO QUE CONTIENE EL DATO DEL NODO
    int AVL::Altura(string dat){
    int altura = 0;
    actual = raiz;

    //MIENTRAS PUEDE APARECER, YA QUE QUEDAN NODOS POR MIRAR
    while(!Vacio(actual)) {
      if(dat == actual->dato.codigo) return altura; // dato encontrado
      else {

        //INCREMENTAMOS LA ALTURA Y SEGUIMOS BUSCANDO
         altura++;
         if(dat > actual->dato.codigo) actual = actual->derecho;
         else if(dat < actual->dato.codigo) actual = actual->izquierdo;
        }
    }

    //SI NO LO ENCUENTRA ES PORQUE NO ESTA EN EL ARBOL
    return -1;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA CONTAR EL NUMERO DE NODOS EN EL ARBOL
    const int AVL::NumeroNodos(){
    contador = 0;
    auxContador(raiz);
    return contador;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION AUXILIAR PARA CONTAR LOS NODOS
    void AVL::auxContador(Nodo *nodo){
    contador++;
    if(nodo->izquierdo) auxContador(nodo->izquierdo);
    if(nodo->derecho)   auxContador(nodo->derecho);
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA CALCULAR LA ALTURA DEL ARBOL HACIENDO REFERENCIA AL NODO DE MAYOR ALTURA
    const int AVL::AlturaArbol(){
    altura = 0;
    auxAltura(raiz, 0);
    return altura;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION AUXILIAR PARA CALCULAR LA ALTURA
    void AVL::auxAltura(Nodo *nodo, int a){
    if(nodo->izquierdo) auxAltura(nodo->izquierdo, a+1);
    if(nodo->derecho)   auxAltura(nodo->derecho, a+1);
    if(EsHoja(nodo) && a > altura) altura = a;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA MOSTRAR RECORRIDOS DEL ARBOL
    void Mostrar(DATO &dat, int FE){
        cout<<"\n\t\t"<<dat.codigo<<" ";
        cout<<dat.carnet<<" ";
        cout<<dat.nombre<<" ";
        cout<<dat.sexo;
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA SEPARAR LAS TRADUCCIONES
    string* AVL::split(string texto){
        static string arreglo[5];
        string aux;
        int j=0;

        for(int i=0; i<texto.length(); i++){
            if(texto[i]=='|'){
                arreglo[j] = aux;
                aux = "";
                j++;
                }else{
                aux+= texto[i];
            }
        }
    return arreglo;
    }
/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA CARGAR EL ARBOL CARNET
    void AVL::cargarArbol_Carnet(class AVL& estru){
        ifstream lectura;
        string cadena;
        DATO aux;
        lectura.open(nomArchivo,ios::in);
        if(lectura.is_open()){
            while(!lectura.eof()){
                getline(lectura,cadena);
                aux.codigo  = *(split(cadena));
                aux.carnet  = *(split(cadena)+1);
                aux.nombre  = *(split(cadena)+2);
                aux.sexo    = *(split(cadena)+3);
                estru.Insertar(aux);
            }
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA CARGAR EL ARBOL NOMBRE
    void AVL::cargarArbol_Nombre(class AVL& estru){
        ifstream lectura;
        string cadena, codigo, carnet, nombre, sexo;
        DATO aux;
        lectura.open(nomArchivo,ios::in);
        if(lectura.is_open()){
            while(!lectura.eof()){
                getline(lectura,cadena);
                codigo   = *(split(cadena));
                carnet   = *(split(cadena)+1);
                nombre   = *(split(cadena)+2);
                sexo     = *(split(cadena)+3);

                aux.codigo = carnet;
                aux.nombre = nombre;
                aux.carnet = codigo;
                aux.sexo  = sexo;
                estru.Insertar(aux);
            }
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA CARGAR EL ARBOL CODIGO
    void AVL::cargarArbol_Codigo(class AVL& estru){
        ifstream lectura;
        string cadena, codigo, carnet, nombre, sexo;
        DATO aux;
        lectura.open(nomArchivo,ios::in);
        if(lectura.is_open()){
            while(!lectura.eof()){
                getline(lectura,cadena);
                codigo  = *(split(cadena));
                carnet  = *(split(cadena)+1);
                nombre  = *(split(cadena)+2);
                sexo    = *(split(cadena)+3);

                aux.codigo = nombre;
                aux.nombre = carnet;
                aux.carnet = codigo;
                aux.sexo   = sexo;
                estru.Insertar(aux);
            }
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA ALMACENAR EL REGISTRO EN EL TXT
    void AVL::almacenar(string cadena){
        ofstream escritura;
        escritura.open(nomArchivo,ios::out|ios::app);
        if(escritura.is_open()){
            escritura<<cadena<<endl;
        }else{
            cout<<"\t\tNO SE PUDO ALMACENAR EL REGISTRO...\n";
        }
        escritura.close();
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA INSERTAR UN NUEVO REGISTRO
    void AVL::registrar(class AVL& estru ){
        DATO aux;
        string cadena, opc;
        cout<<"\n\t\tINGRESAR CODIGO (8 digitos)->: "; cin>>aux.codigo;
        cout<<  "\t\tINGRESAR NOMBRE    -> : "; cin>>aux.nombre;
        cout<<  "\t\tINGRESAR EDAD      -> : "; cin>>aux.carnet;
        cout<<  "\t\tINGRESAR SEXO      -> : "; cin>>aux.sexo;

        cadena+=aux.codigo+"|";
        cadena+=aux.nombre+"|";
        cadena+=aux.carnet+"|";
        cadena+=aux.sexo+"|";
        cout<<"\n\t\tESTA SEGURO DE GUARDAR REGISTRO (S/N) ->: ";
        cin>>opc;
        if(opc=="s"||opc=="S"){
            estru.almacenar(cadena);
            estru.Insertar(aux);
            cout<<"\n\t\tREGISTRO  GUARDADO CON EXITO...\n";
        }else{
        cout<<"\n\t\tPROCESO FUE CANCELADO CON EXITO...\n";
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA BUSCAR UN REGISTRO
    DATO AVL::buscarRegistro(string cod){
        actual = raiz;
        while(!Vacio(actual)) {
        if(cod == actual->dato.codigo) return actual->dato;
        else if(cod > actual->dato.codigo) actual = actual->derecho;
        else if(cod < actual->dato.codigo) actual = actual->izquierdo;
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA BUSCAR UN REGISTRO EN EL ARBOL
    void AVL::buscar(class AVL& estru){

        string cod;
        DATO aux;
        bool bandera;
        cout<<"\n\tDIGITAR CODIGO DEL REGISTRO -> :";
        getline(cin,cod);
        bandera = Buscar(cod);
        if(bandera == true){
            aux = estru.buscarRegistro(cod);
            cout<<"\n\t\tREGISTRO ENCONTRADO\n";
            Mostrar(aux,0);
        }else{
            cout<<"\n\t\tEL CODIGO NO EXISTE...\n";
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA ELIMINAR EL REGISTRO DEL TXT
    void AVL::eliminarEnArchivo(string cadena){
        ifstream lectura;
        ofstream escritura;
        string aux;
        lectura.open(nomArchivo,ios::in);
        escritura.open("auxiliar.txt",ios::out|ios::app);
        if(lectura.is_open()){
            while(!lectura.eof()){
                getline(lectura,aux);
                if(cadena==aux){
                    cout<<"\n\t\tREGISTRO ELIMINADO...\n";
                }else{
                    escritura<<aux<<endl;
                }
            }
        }
        lectura.close();
        escritura.close();
        remove(nomArchivo);
        rename("auxiliar.txt",nomArchivo);
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PARA ELIMINAR UN REGISTRO DEL ARBOL
    void AVL::eliminar(class AVL& estru){
        string cod, cadena,opc;
        DATO aux;
        bool band;
        cout<<"\n\t\tDIGITAR CODIGO QUE DESEA ELIMINAR ->: "; cin>>cod;
        band = estru.Buscar(cod);
        if(band ==true){
            aux = estru.buscarRegistro(cod);
            cadena += aux.codigo+"|";
            cadena += aux.nombre+"|";
            cadena += aux.carnet+"|";
            cadena += aux.sexo+"|";
            cout<<"\n\t\tDATOS DEL REGISTRO ENCONTRADO\n";
            Mostrar(aux,0);
            cout<<"\n";
            cout<<"\n\t\tESTA SEGURO DE ELIMINAR REGISTRO (S/N)->: ";
            cin>>opc;
            if(opc=="s"||opc=="S"){
               estru.eliminarEnArchivo(cadena);
               estru.Borrar(cod);
            }else{
                cout<<"\n\t\tPROCESO CANCELADO CON EXITO...\n";
            }
        }else{
            cout<<"\n\t\tEL CODIGO NO EXISTE...\n";
        }
    }

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION QUE NOS MUESTRA EL MENU DE OPCIONES DISPONIBLES DEL ROGRAMA
    void AVL::menu(class AVL &codigo,class AVL &carnet,class AVL &nombre){


        char opcion;

        do{
        system("CLS");
        cout<<"\n\t1.   INSERTAR REGISTRO"<<endl
            <<  "\t2.   BUSCAR REGISTRO"<<endl
            <<  "\t3.   BORRAR REGISTRO"<<endl
            <<  "\t4.   MOSTRAR ARBOL EN ORDEN"<<endl
            <<  "\t5.   MOSTRAR ARBOL PRE ORDEN"<<endl
            <<  "\t6.   MOSTRAR ARBOL POST ORDEN"<<endl
            <<  "\t0.   SALIR"<<endl
            <<  "\t     DIGITE UNA OPCION ->: "; cin>>opcion; cin.ignore();

        switch(opcion){
        case '0':
            codigo.Podar(raiz);
            carnet.Podar(raiz);
            nombre.Podar(raiz);
            exit(1);  break;
        case '1':
            system("CLS");
            codigo.registrar(codigo);
            getch();
            break;
        case '2':
            system("CLS");
            cout<<"\n\tCOMO DESEA REALIZAR LA BUSQUEDA"<<endl
                <<"\t1.POR NOMBRE COMPLETO"<<endl
                <<"\t2.POR CORRELATIVO"<<endl
                <<"\t3.POR CARNET"<<endl
                <<"\tDIGITE OPCION -> "; cin>>opcion; cin.ignore();

            if(opcion=='1')codigo.buscar(codigo);
            if(opcion=='2')carnet.buscar(carnet);
            if(opcion=='3')nombre.buscar(nombre);
            getch();
            break;
        case '3':
            system("CLS");
            codigo.eliminar(codigo);
            getch();
            break;
        case '4':
            system("CLS");
            cout<<"\n\t\tEN ORDEN\n";
            codigo.InOrden(Mostrar);
            getch();        break;
        case '5':
            system("CLS");
            cout<<"\n\t\tPRE ORDEN\n";
            codigo.PreOrden(Mostrar);
            getch();        break;
        case '6':
            system("CLS");
            cout<<"\n\t\tPOST ORDEN\n";
            codigo.PostOrden(Mostrar);
            getch();        break;
        }
     }while(opcion != '0');
}

/*-----------------------------------------------------------------------------------------------------*/
    //FUNCION PRINCIPAL DEL SISTEMA
    int main(){

    AVL arbolCodigo;
    AVL arbolCarnet;
    AVL arbolNombre;

    arbolCodigo.cargarArbol_Codigo(arbolCodigo);
    arbolCarnet.cargarArbol_Carnet(arbolCarnet);
    arbolNombre.cargarArbol_Nombre(arbolNombre);

    arbolCodigo.menu(arbolCodigo,arbolCarnet,arbolNombre);

    getch();
    return 0;
    }

