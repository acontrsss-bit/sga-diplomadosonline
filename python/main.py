import os

# --- ESTRUCTURAS DE DATOS ---
class Pila:
    def __init__(self): self.items = []
    def apilar(self, item): self.items.append(item)
    def desapilar(self): return self.items.pop() if self.items else None
    def vacia(self): return len(self.items) == 0

class Cola:
    def __init__(self): self.items = []
    def encolar(self, item): self.items.append(item)
    def desencolar(self): return self.items.pop(0) if self.items else None
    def vacia(self): return len(self.items) == 0

# --- MODELOS Y POLIMORFISMO ---
class Persona:
    def __init__(self, cedula, nombre, correo):
        self.cedula = cedula
        self.nombre = nombre
        self.correo = correo

class ProgramaAcademico:
    def evaluarAprobacion(self, notas):
        pass

class Curso(ProgramaAcademico):
    def evaluarAprobacion(self, notas):
        return sum(notas) / 3.0 >= 10.0

class Diplomado(ProgramaAcademico):
    def evaluarAprobacion(self, notas):
        return sum(notas) / 3.0 >= 14.0

class Bootcamp(ProgramaAcademico):
    def evaluarAprobacion(self, notas):
        return all(n >= 14.0 for n in notas)

class Alumno(Persona):
    def __init__(self, cedula, nombre, correo, tipo_programa, notas=None):
        super().__init__(cedula, nombre, correo)
        self.tipo_programa = tipo_programa
        
        if tipo_programa == "Curso": self.programa = Curso()
        elif tipo_programa == "Diplomado": self.programa = Diplomado()
        elif tipo_programa == "Bootcamp": self.programa = Bootcamp()
        else: self.programa = Curso()
        
        self.notas = notas if notas else [0.0, 0.0, 0.0]

    def esta_aprobado(self):
        return self.programa.evaluarAprobacion(self.notas)
        
    def promedio(self):
        return sum(self.notas) / 3.0

class Profesor(Persona):
    def __init__(self, cedula, nombre, correo, especialidad, materia):
        super().__init__(cedula, nombre, correo)
        self.especialidad = especialidad
        self.materia = materia

# --- PERSISTENCIA ---
class Gestor:
    @staticmethod
    def cargar_alumnos():
        alumnos = []
        if os.path.exists("alumnos.txt"):
            with open("alumnos.txt", "r", encoding="utf-8") as f:
                for linea in f:
                    p = linea.strip().split(',')
                    if len(p) == 7:
                        alumnos.append(Alumno(p[0], p[1], p[2], p[3], [float(p[4]), float(p[5]), float(p[6])]))
        return alumnos

    @staticmethod
    def guardar_alumnos(alumnos):
        with open("alumnos.txt", "w", encoding="utf-8") as f:
            for a in alumnos:
                f.write(f"{a.cedula},{a.nombre},{a.correo},{a.tipo_programa},{a.notas[0]},{a.notas[1]},{a.notas[2]}\n")

    @staticmethod
    def cargar_profesores():
        profesores = []
        if os.path.exists("profesores.txt"):
            with open("profesores.txt", "r", encoding="utf-8") as f:
                for linea in f:
                    p = linea.strip().split(',')
                    if len(p) >= 5:
                        profesores.append(Profesor(p[0], p[1], p[2], p[3], p[4]))
        return profesores

    @staticmethod
    def guardar_profesores(profesores):
        with open("profesores.txt", "w", encoding="utf-8") as f:
            for p in profesores:
                f.write(f"{p.cedula},{p.nombre},{p.correo},{p.especialidad},{p.materia}\n")

# --- CONTROLADOR ---
class App:
    def __init__(self):
        self.pila_notas = Pila()
        
    def menu(self):
        while True:
            print("==================================================")
            print("SGA-DO: SISTEMA DIPLOMADOSONLINE")
            print("==================================================")
            print("1. Registrar Alumno")
            print("2. Registrar Profesor")
            print("3. Registrar Notas a un Alumno")
            print("4. Deshacer Último Registro de Nota")
            print("5. Generar Cola de Certificados")
            print("6. Mostrar Reporte General")
            print("7. Salir")
            print("==================================================")
            
            opcion_str = input("Seleccione una opción (1-7): ").strip()
            
            try:
                opc = int(opcion_str)
            except ValueError:
                print("Error: Ingrese un valor numérico válido")
                continue
                
            if opc == 1:
                cedula = input("Cédula: ").strip()
                nombre = input("Nombre: ").strip()
                correo = input("Correo: ").strip()
                prog = input("Programa (Curso/Diplomado/Bootcamp): ").strip()
                alumnos = Gestor.cargar_alumnos()
                alumnos.append(Alumno(cedula, nombre, correo, prog))
                Gestor.guardar_alumnos(alumnos)
                print("[ÉXITO] Alumno registrado.\n")
                
            elif opc == 2:
                cedula = input("Cédula: ").strip()
                nombre = input("Nombre: ").strip()
                correo = input("Correo: ").strip()
                especialidad = input("Especialidad: ").strip()
                materia = input("Materia: ").strip()
                profesores = Gestor.cargar_profesores()
                profesores.append(Profesor(cedula, nombre, correo, especialidad, materia))
                Gestor.guardar_profesores(profesores)
                print("[ÉXITO] Profesor registrado.\n")
                
            elif opc == 3:
                cedula = input("Cédula del alumno: ").strip()
                alumnos = Gestor.cargar_alumnos()
                alumno = next((a for a in alumnos if a.cedula == cedula), None)
                if alumno:
                    try:
                        idx = int(input("Índice de nota a modificar (1-3): ").strip()) - 1
                        nota_str = input("Nota (0-20): ").strip().replace(',', '.')
                        nota = float(nota_str)
                        if 0 <= idx <= 2 and 0 <= nota <= 20:
                            self.pila_notas.apilar((cedula, idx, alumno.notas[idx]))
                            alumno.notas[idx] = nota
                            Gestor.guardar_alumnos(alumnos)
                            print("[ÉXITO] Nota registrada.\n")
                        else:
                            print("[ERROR] Índice o nota fuera de rango.\n")
                    except ValueError:
                        print("Error: Ingrese un valor numérico válido\n")
                else:
                    print("[ERROR] Alumno no encontrado.\n")
                    
            elif opc == 4:
                tx = self.pila_notas.desapilar()
                if tx:
                    ced, idx, ant = tx
                    alumnos = Gestor.cargar_alumnos()
                    alumno = next((a for a in alumnos if a.cedula == ced), None)
                    if alumno:
                        alumno.notas[idx] = ant
                        Gestor.guardar_alumnos(alumnos)
                        print("[ÉXITO] Última nota deshecha.\n")
                else:
                    print("[INFO] Nada que deshacer.\n")
                    
            elif opc == 5:
                alumnos = Gestor.cargar_alumnos()
                cola = Cola()
                for a in alumnos:
                    if a.esta_aprobado():
                        cola.encolar(a)
                
                contador = 0
                certificados = []
                while not cola.vacia():
                    a = cola.desencolar()
                    contador += 1
                    status_text = "APROBADO (Cumple regla de ninguna nota < 14)" if a.tipo_programa == "Bootcamp" else "APROBADO"
                    certificados.append(f"{contador}. [{a.cedula}] {a.nombre}\n - Programa: {a.tipo_programa}\n - Promedio Final: {a.promedio():.1f}\n - Estatus: {status_text}\n")
                
                with open("certificados_pendientes.txt", "w", encoding="utf-8") as f:
                    f.write("=========================================\n")
                    f.write("REPORTE DE CERTIFICADOS PENDIENTES\n")
                    f.write("=========================================\n")
                    f.write(f"Total de graduandos en cola: {contador}\n\n")
                    for cert in certificados:
                        f.write(cert + "\n")
                    f.write("=========================================\n")
                    f.write("* Fin del reporte - Generado por SGA-DO *\n")
                    
                print("[ÉXITO] Cola de certificados generada (certificados_pendientes.txt).\n")
                
            elif opc == 6:
                print("\n--- REPORTE GENERAL ---")
                profesores = Gestor.cargar_profesores()
                print("PROFESORES ACTIVOS:")
                for p in profesores:
                    print(f"[{p.cedula}] {p.nombre} - {p.especialidad} / {p.materia}")
                
                alumnos = Gestor.cargar_alumnos()
                print("\nALUMNOS REGISTRADOS:")
                for a in alumnos:
                    estado = "APROBADO" if a.esta_aprobado() else "REPROBADO"
                    print(f"[{a.cedula}] {a.nombre} - {a.tipo_programa} - Promedio: {a.promedio():.1f} -> {estado}")
                print("-----------------------\n")
                
            elif opc == 7:
                break
            else:
                print("Error: Ingrese un valor numérico válido\n")

if __name__ == "__main__":
    App().menu()
