#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

// --- ESTRUCTURAS DE DATOS (Gestión Manual de Memoria) ---
template <typename T>
class Nodo {
public:
    T dato;
    Nodo* siguiente;
    Nodo(T d) : dato(d), siguiente(nullptr) {}
};

template <typename T>
class Pila {
private:
    Nodo<T>* tope;
public:
    Pila() : tope(nullptr) {}
    ~Pila() {
        while(!vacia()) desapilar();
    }
    void apilar(T item) {
        Nodo<T>* nuevo = new Nodo<T>(item);
        nuevo->siguiente = tope;
        tope = nuevo;
    }
    T desapilar() {
        if(vacia()) return T();
        Nodo<T>* temp = tope;
        T dato = temp->dato;
        tope = tope->siguiente;
        delete temp;
        return dato;
    }
    bool vacia() const { return tope == nullptr; }
};

template <typename T>
class Cola {
private:
    Nodo<T>* frente;
    Nodo<T>* fin;
    int size;
public:
    Cola() : frente(nullptr), fin(nullptr), size(0) {}
    ~Cola() {
        while(!vacia()) desencolar();
    }
    void encolar(T item) {
        Nodo<T>* nuevo = new Nodo<T>(item);
        if(vacia()) {
            frente = fin = nuevo;
        } else {
            fin->siguiente = nuevo;
            fin = nuevo;
        }
        size++;
    }
    T desencolar() {
        if(vacia()) return T();
        Nodo<T>* temp = frente;
        T dato = temp->dato;
        frente = frente->siguiente;
        if(frente == nullptr) fin = nullptr;
        delete temp;
        size--;
        return dato;
    }
    bool vacia() const { return frente == nullptr; }
    int getSize() const { return size; }
};

// --- MODELOS Y POLIMORFISMO ---
class Persona {
protected:
    string cedula;
    string nombre;
    string correo;
public:
    Persona(string c, string n, string co) : cedula(c), nombre(n), correo(co) {}
    virtual ~Persona() {}
    string getCedula() const { return cedula; }
    string getNombre() const { return nombre; }
    string getCorreo() const { return correo; }
};

class ProgramaAcademico {
public:
    virtual bool evaluarAprobacion(const vector<float>& notas) = 0;
    virtual ~ProgramaAcademico() {}
};

class Curso : public ProgramaAcademico {
public:
    bool evaluarAprobacion(const vector<float>& notas) override {
        float sum = 0;
        for (float n : notas) sum += n;
        return (sum / 3.0f) >= 10.0f;
    }
};

class Diplomado : public ProgramaAcademico {
public:
    bool evaluarAprobacion(const vector<float>& notas) override {
        float sum = 0;
        for (float n : notas) sum += n;
        return (sum / 3.0f) >= 14.0f;
    }
};

class Bootcamp : public ProgramaAcademico {
public:
    bool evaluarAprobacion(const vector<float>& notas) override {
        for (float n : notas) {
            if (n < 14.0f) return false;
        }
        return true;
    }
};

class Alumno : public Persona {
private:
    string tipo_programa;
    ProgramaAcademico* programa;
    vector<float> notas;
public:
    Alumno(string c, string n, string co, string tipo, vector<float> nts = {0,0,0}) 
        : Persona(c, n, co), tipo_programa(tipo), notas(nts) {
        if (tipo == "Curso") programa = new Curso();
        else if (tipo == "Diplomado") programa = new Diplomado();
        else if (tipo == "Bootcamp") programa = new Bootcamp();
        else programa = new Curso();
    }
    
    Alumno(const Alumno& other) : Persona(other.cedula, other.nombre, other.correo) {
        tipo_programa = other.tipo_programa;
        notas = other.notas;
        if (tipo_programa == "Curso") programa = new Curso();
        else if (tipo_programa == "Diplomado") programa = new Diplomado();
        else if (tipo_programa == "Bootcamp") programa = new Bootcamp();
        else programa = new Curso();
    }

    ~Alumno() {
        delete programa;
    }

    bool estaAprobado() {
        return programa->evaluarAprobacion(notas);
    }
    
    float promedio() {
        float sum = 0;
        for (float n : notas) sum += n;
        return sum / 3.0f;
    }

    string getTipoPrograma() const { return tipo_programa; }
    vector<float> getNotas() const { return notas; }
    void setNota(int idx, float val) {
        if(idx >= 0 && idx < 3) notas[idx] = val;
    }
};

class Profesor : public Persona {
private:
    string especialidad;
    string materia;
public:
    Profesor(string c, string n, string co, string esp, string mat) 
        : Persona(c, n, co), especialidad(esp), materia(mat) {}
    string getEspecialidad() const { return especialidad; }
    string getMateria() const { return materia; }
};

struct TxNota {
    string cedula;
    int idx;
    float ant;
    bool valida = false;
};

// --- PERSISTENCIA ---
class Gestor {
public:
    static vector<Alumno*> cargarAlumnos() {
        vector<Alumno*> lista;
        ifstream f("alumnos.txt");
        if (!f.is_open()) return lista;
        string linea, p;
        while (getline(f, linea)) {
            stringstream ss(linea);
            vector<string> campos;
            while (getline(ss, p, ',')) campos.push_back(p);
            if (campos.size() == 7) {
                lista.push_back(new Alumno(campos[0], campos[1], campos[2], campos[3],
                                       {stof(campos[4]), stof(campos[5]), stof(campos[6])}));
            }
        }
        return lista;
    }

    static void guardarAlumnos(const vector<Alumno*>& lista) {
        ofstream f("alumnos.txt");
        for (const auto& a : lista) {
            vector<float> n = a->getNotas();
            f << a->getCedula() << "," << a->getNombre() << "," << a->getCorreo() << "," 
              << a->getTipoPrograma() << "," << n[0] << "," << n[1] << "," << n[2] << "\n";
        }
    }

    static vector<Profesor*> cargarProfesores() {
        vector<Profesor*> lista;
        ifstream f("profesores.txt");
        if (!f.is_open()) return lista;
        string linea, p;
        while (getline(f, linea)) {
            stringstream ss(linea);
            vector<string> campos;
            while (getline(ss, p, ',')) campos.push_back(p);
            if (campos.size() >= 5) {
                lista.push_back(new Profesor(campos[0], campos[1], campos[2], campos[3], campos[4]));
            }
        }
        return lista;
    }

    static void guardarProfesores(const vector<Profesor*>& lista) {
        ofstream f("profesores.txt");
        for (const auto& p : lista) {
            f << p->getCedula() << "," << p->getNombre() << "," << p->getCorreo() << "," 
              << p->getEspecialidad() << "," << p->getMateria() << "\n";
        }
    }

    static void liberarAlumnos(vector<Alumno*>& lista) {
        for(auto a : lista) delete a;
        lista.clear();
    }

    static void liberarProfesores(vector<Profesor*>& lista) {
        for(auto p : lista) delete p;
        lista.clear();
    }
};

// --- CONTROLADOR ---
int main() {
    Pila<TxNota> pilaNotas;
    string opc_str;
    
    while (true) {
        cout << "==================================================\n";
        cout << "SGA-DO: SISTEMA DIPLOMADOSONLINE\n";
        cout << "==================================================\n";
        cout << "1. Registrar Alumno\n";
        cout << "2. Registrar Profesor\n";
        cout << "3. Registrar Notas a un Alumno\n";
        cout << "4. Deshacer Ultimo Registro de Nota\n";
        cout << "5. Generar Cola de Certificados\n";
        cout << "6. Mostrar Reporte General\n";
        cout << "7. Salir\n";
        cout << "==================================================\n";
        cout << "Seleccione una opcion (1-7): ";
        
        getline(cin, opc_str);
        
        int opc;
        try {
            opc = stoi(opc_str);
        } catch (...) {
            cout << "Error: Ingrese un valor numerico valido\n\n";
            continue;
        }
        
        if (opc == 1) {
            string c, n, co, p;
            cout << "Cedula: "; getline(cin, c);
            cout << "Nombre: "; getline(cin, n);
            cout << "Correo: "; getline(cin, co);
            cout << "Programa (Curso/Diplomado/Bootcamp): "; getline(cin, p);
            
            auto lista = Gestor::cargarAlumnos();
            lista.push_back(new Alumno(c, n, co, p));
            Gestor::guardarAlumnos(lista);
            Gestor::liberarAlumnos(lista);
            cout << "[EXITO] Registrado.\n\n";
            
        } else if (opc == 2) {
            string c, n, co, esp, mat;
            cout << "Cedula: "; getline(cin, c);
            cout << "Nombre: "; getline(cin, n);
            cout << "Correo: "; getline(cin, co);
            cout << "Especialidad: "; getline(cin, esp);
            cout << "Materia: "; getline(cin, mat);

            auto lista = Gestor::cargarProfesores();
            lista.push_back(new Profesor(c, n, co, esp, mat));
            Gestor::guardarProfesores(lista);
            Gestor::liberarProfesores(lista);
            cout << "[EXITO] Registrado.\n\n";

        } else if (opc == 3) {
            string c;
            cout << "Cedula del alumno: "; getline(cin, c);
            auto lista = Gestor::cargarAlumnos();
            bool eq = false;
            for (auto a : lista) {
                if (a->getCedula() == c) {
                    eq = true;
                    string idx_str, nota_str;
                    cout << "Indice (1-3): "; getline(cin, idx_str);
                    cout << "Nota (0-20): "; getline(cin, nota_str);
                    try {
                        int idx = stoi(idx_str) - 1;
                        float nota = stof(nota_str);
                        if (idx >= 0 && idx <= 2 && nota >= 0 && nota <= 20) {
                            TxNota tx = {c, idx, a->getNotas()[idx], true};
                            pilaNotas.apilar(tx);
                            a->setNota(idx, nota);
                            Gestor::guardarAlumnos(lista);
                            cout << "[EXITO] Nota registrada.\n\n";
                        } else {
                            cout << "[ERROR] Rango invalido.\n\n";
                        }
                    } catch (...) {
                        cout << "Error: Ingrese un valor numerico valido\n\n";
                    }
                    break;
                }
            }
            if (!eq) cout << "[ERROR] No encontrado.\n\n";
            Gestor::liberarAlumnos(lista);
            
        } else if (opc == 4) {
            if (!pilaNotas.vacia()) {
                TxNota tx = pilaNotas.desapilar();
                if(tx.valida) {
                    auto lista = Gestor::cargarAlumnos();
                    for (auto a : lista) {
                        if (a->getCedula() == tx.cedula) {
                            a->setNota(tx.idx, tx.ant);
                            Gestor::guardarAlumnos(lista);
                            cout << "[EXITO] Deshecho.\n\n";
                            break;
                        }
                    }
                    Gestor::liberarAlumnos(lista);
                }
            } else {
                cout << "[INFO] Nada que deshacer.\n\n";
            }
            
        } else if (opc == 5) {
            auto lista = Gestor::cargarAlumnos();
            Cola<Alumno*> cola;
            for (auto a : lista) {
                if (a->estaAprobado()) cola.encolar(new Alumno(*a));
            }
            
            ofstream f("certificados_pendientes.txt");
            f << "=========================================\n";
            f << "REPORTE DE CERTIFICADOS PENDIENTES\n";
            f << "=========================================\n";
            f << "Total de graduandos en cola: " << cola.getSize() << "\n\n";

            int cont = 1;
            while (!cola.vacia()) {
                Alumno* a = cola.desencolar();
                string status_text = (a->getTipoPrograma() == "Bootcamp") ? "APROBADO (Cumple regla de ninguna nota < 14)" : "APROBADO";
                f << cont << ". [" << a->getCedula() << "] " << a->getNombre() << "\n"
                  << " - Programa: " << a->getTipoPrograma() << "\n"
                  << " - Promedio Final: " << a->promedio() << "\n"
                  << " - Estatus: " << status_text << "\n\n";
                cont++;
                delete a; // important: release cloned instance
            }
            f << "=========================================\n";
            f << "* Fin del reporte - Generado por SGA-DO *\n";
            f.close();
            Gestor::liberarAlumnos(lista);
            cout << "[EXITO] Generados.\n\n";
            
        } else if (opc == 6) {
            cout << "\n--- REPORTE GENERAL ---\n";
            auto profes = Gestor::cargarProfesores();
            cout << "PROFESORES ACTIVOS:\n";
            for (auto p : profes) {
                cout << "[" << p->getCedula() << "] " << p->getNombre() << " - " << p->getEspecialidad() << " / " << p->getMateria() << "\n";
            }
            Gestor::liberarProfesores(profes);
            
            auto lista = Gestor::cargarAlumnos();
            cout << "\nALUMNOS REGISTRADOS:\n";
            for (auto a : lista) {
                string estado = a->estaAprobado() ? "APROBADO" : "REPROBADO";
                cout << "[" << a->getCedula() << "] " << a->getNombre() << " - " << a->getTipoPrograma() << " - Promedio: " << a->promedio() << " -> " << estado << "\n";
            }
            Gestor::liberarAlumnos(lista);
            cout << "-----------------------\n\n";
            
        } else if (opc == 7) {
            break;
            
        } else {
            cout << "Error: Ingrese un valor numerico valido\n\n";
        }
    }
    return 0;
}
