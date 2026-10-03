package java;

import java.io.*;
import java.util.*;

// --- MODELOS Y POLIMORFISMO ---
abstract class Persona {
    protected String cedula;
    protected String nombre;
    protected String correo;

    public Persona(String cedula, String nombre, String correo) {
        this.cedula = cedula;
        this.nombre = nombre;
        this.correo = correo;
    }

    public String getCedula() { return cedula; }
    public String getNombre() { return nombre; }
    public String getCorreo() { return correo; }
}

abstract class ProgramaAcademico {
    public abstract boolean evaluarAprobacion(List<Double> notas);
}

class Curso extends ProgramaAcademico {
    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        double sum = 0;
        for (double n : notas) sum += n;
        return (sum / 3.0) >= 10.0;
    }
}

class Diplomado extends ProgramaAcademico {
    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        double sum = 0;
        for (double n : notas) sum += n;
        return (sum / 3.0) >= 14.0;
    }
}

class Bootcamp extends ProgramaAcademico {
    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        for (double n : notas) {
            if (n < 14.0) return false;
        }
        return true;
    }
}

class Alumno extends Persona {
    private String tipoPrograma;
    private ProgramaAcademico programa;
    private List<Double> notas;

    public Alumno(String cedula, String nombre, String correo, String tipoPrograma, List<Double> notas) {
        super(cedula, nombre, correo);
        this.tipoPrograma = tipoPrograma;
        this.notas = new ArrayList<>(notas);
        if (tipoPrograma.equals("Curso")) this.programa = new Curso();
        else if (tipoPrograma.equals("Diplomado")) this.programa = new Diplomado();
        else if (tipoPrograma.equals("Bootcamp")) this.programa = new Bootcamp();
        else this.programa = new Curso();
    }

    public boolean estaAprobado() {
        return programa.evaluarAprobacion(notas);
    }

    public double promedio() {
        double sum = 0;
        for (double n : notas) sum += n;
        return sum / 3.0;
    }

    public String getTipoPrograma() { return tipoPrograma; }
    public List<Double> getNotas() { return notas; }
    public void setNota(int idx, double val) {
        if(idx >= 0 && idx < 3) notas.set(idx, val);
    }
}

class Profesor extends Persona {
    private String especialidad;
    private String materia;

    public Profesor(String cedula, String nombre, String correo, String especialidad, String materia) {
        super(cedula, nombre, correo);
        this.especialidad = especialidad;
        this.materia = materia;
    }

    public String getEspecialidad() { return especialidad; }
    public String getMateria() { return materia; }
}

class TxNota {
    public String cedula;
    public int idx;
    public double ant;
    public TxNota(String c, int i, double a) {
        this.cedula = c; this.idx = i; this.ant = a;
    }
}

// --- PERSISTENCIA ---
class Gestor {
    public static List<Alumno> cargarAlumnos() {
        List<Alumno> lista = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader("alumnos.txt"))) {
            String linea;
            while ((linea = br.readLine()) != null) {
                String[] p = linea.split(",");
                if (p.length == 7) {
                    List<Double> notas = Arrays.asList(Double.parseDouble(p[4]), Double.parseDouble(p[5]), Double.parseDouble(p[6]));
                    lista.add(new Alumno(p[0], p[1], p[2], p[3], notas));
                }
            }
        } catch (IOException e) {
            // ignore si no existe
        }
        return lista;
    }

    public static void guardarAlumnos(List<Alumno> lista) {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter("alumnos.txt"))) {
            for (Alumno a : lista) {
                bw.write(a.getCedula() + "," + a.getNombre() + "," + a.getCorreo() + "," +
                         a.getTipoPrograma() + "," + a.getNotas().get(0) + "," + 
                         a.getNotas().get(1) + "," + a.getNotas().get(2));
                bw.newLine();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static List<Profesor> cargarProfesores() {
        List<Profesor> lista = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader("profesores.txt"))) {
            String linea;
            while ((linea = br.readLine()) != null) {
                String[] p = linea.split(",");
                if (p.length >= 5) {
                    lista.add(new Profesor(p[0], p[1], p[2], p[3], p[4]));
                }
            }
        } catch (IOException e) {
            // ignore
        }
        return lista;
    }

    public static void guardarProfesores(List<Profesor> lista) {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter("profesores.txt"))) {
            for (Profesor p : lista) {
                bw.write(p.getCedula() + "," + p.getNombre() + "," + p.getCorreo() + "," +
                         p.getEspecialidad() + "," + p.getMateria());
                bw.newLine();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

// --- CONTROLADOR ---
public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Stack<TxNota> pilaNotas = new Stack<>();

        while (true) {
            System.out.println("==================================================");
            System.out.println("SGA-DO: SISTEMA DIPLOMADOSONLINE");
            System.out.println("==================================================");
            System.out.println("1. Registrar Alumno");
            System.out.println("2. Registrar Profesor");
            System.out.println("3. Registrar Notas a un Alumno");
            System.out.println("4. Deshacer Último Registro de Nota");
            System.out.println("5. Generar Cola de Certificados");
            System.out.println("6. Mostrar Reporte General");
            System.out.println("7. Salir");
            System.out.println("==================================================");
            System.out.print("Seleccione una opción (1-7): ");

            String opcStr = scanner.nextLine();
            int opc;
            try {
                opc = Integer.parseInt(opcStr);
            } catch (NumberFormatException e) {
                System.out.println("Error: Ingrese un valor numérico válido\n");
                continue;
            }

            if (opc == 1) {
                System.out.print("Cédula: "); String c = scanner.nextLine();
                System.out.print("Nombre: "); String n = scanner.nextLine();
                System.out.print("Correo: "); String co = scanner.nextLine();
                System.out.print("Programa (Curso/Diplomado/Bootcamp): "); String p = scanner.nextLine();

                List<Alumno> lista = Gestor.cargarAlumnos();
                lista.add(new Alumno(c, n, co, p, Arrays.asList(0.0, 0.0, 0.0)));
                Gestor.guardarAlumnos(lista);
                System.out.println("[ÉXITO] Registrado.\n");

            } else if (opc == 2) {
                System.out.print("Cédula: "); String c = scanner.nextLine();
                System.out.print("Nombre: "); String n = scanner.nextLine();
                System.out.print("Correo: "); String co = scanner.nextLine();
                System.out.print("Especialidad: "); String esp = scanner.nextLine();
                System.out.print("Materia: "); String mat = scanner.nextLine();

                List<Profesor> lista = Gestor.cargarProfesores();
                lista.add(new Profesor(c, n, co, esp, mat));
                Gestor.guardarProfesores(lista);
                System.out.println("[ÉXITO] Registrado.\n");

            } else if (opc == 3) {
                System.out.print("Cédula del alumno: "); String c = scanner.nextLine();
                List<Alumno> lista = Gestor.cargarAlumnos();
                boolean encontrado = false;
                for (Alumno a : lista) {
                    if (a.getCedula().equals(c)) {
                        encontrado = true;
                        System.out.print("Índice (1-3): "); String idxStr = scanner.nextLine();
                        System.out.print("Nota (0-20): "); String notaStr = scanner.nextLine();
                        try {
                            int idx = Integer.parseInt(idxStr) - 1;
                            double nota = Double.parseDouble(notaStr);
                            if (idx >= 0 && idx <= 2 && nota >= 0 && nota <= 20) {
                                pilaNotas.push(new TxNota(c, idx, a.getNotas().get(idx)));
                                a.setNota(idx, nota);
                                Gestor.guardarAlumnos(lista);
                                System.out.println("[ÉXITO] Nota registrada.\n");
                            } else {
                                System.out.println("[ERROR] Rango inválido.\n");
                            }
                        } catch (Exception e) {
                            System.out.println("Error: Ingrese un valor numérico válido\n");
                        }
                        break;
                    }
                }
                if (!encontrado) System.out.println("[ERROR] No encontrado.\n");

            } else if (opc == 4) {
                if (!pilaNotas.isEmpty()) {
                    TxNota tx = pilaNotas.pop();
                    List<Alumno> lista = Gestor.cargarAlumnos();
                    for (Alumno a : lista) {
                        if (a.getCedula().equals(tx.cedula)) {
                            a.setNota(tx.idx, tx.ant);
                            Gestor.guardarAlumnos(lista);
                            System.out.println("[ÉXITO] Deshecho.\n");
                            break;
                        }
                    }
                } else {
                    System.out.println("[INFO] Nada que deshacer.\n");
                }

            } else if (opc == 5) {
                List<Alumno> lista = Gestor.cargarAlumnos();
                Queue<Alumno> cola = new LinkedList<>();
                for (Alumno a : lista) {
                    if (a.estaAprobado()) cola.add(a);
                }

                try (BufferedWriter bw = new BufferedWriter(new FileWriter("certificados_pendientes.txt"))) {
                    bw.write("=========================================\n");
                    bw.write("REPORTE DE CERTIFICADOS PENDIENTES\n");
                    bw.write("=========================================\n");
                    bw.write("Total de graduandos en cola: " + cola.size() + "\n\n");

                    int cont = 1;
                    while (!cola.isEmpty()) {
                        Alumno a = cola.poll();
                        String statusText = (a.getTipoPrograma().equals("Bootcamp")) ? "APROBADO (Cumple regla de ninguna nota < 14)" : "APROBADO";
                        bw.write(cont + ". [" + a.getCedula() + "] " + a.getNombre() + "\n");
                        bw.write(" - Programa: " + a.getTipoPrograma() + "\n");
                        bw.write(String.format(Locale.US, " - Promedio Final: %.1f\n", a.promedio()));
                        bw.write(" - Estatus: " + statusText + "\n\n");
                        cont++;
                    }
                    bw.write("=========================================\n");
                    bw.write("* Fin del reporte - Generado por SGA-DO *\n");
                    System.out.println("[ÉXITO] Generados.\n");
                } catch (IOException e) {
                    e.printStackTrace();
                }

            } else if (opc == 6) {
                System.out.println("\n--- REPORTE GENERAL ---");
                List<Profesor> profes = Gestor.cargarProfesores();
                System.out.println("PROFESORES ACTIVOS:");
                for (Profesor p : profes) {
                    System.out.println("[" + p.getCedula() + "] " + p.getNombre() + " - " + p.getEspecialidad() + " / " + p.getMateria());
                }

                List<Alumno> lista = Gestor.cargarAlumnos();
                System.out.println("\nALUMNOS REGISTRADOS:");
                for (Alumno a : lista) {
                    String estado = a.estaAprobado() ? "APROBADO" : "REPROBADO";
                    System.out.printf(Locale.US, "[%s] %s - %s - Promedio: %.1f -> %s\n", 
                                      a.getCedula(), a.getNombre(), a.getTipoPrograma(), a.promedio(), estado);
                }
                System.out.println("-----------------------\n");
            } else if (opc == 7) {
                break;
            } else {
                System.out.println("Error: Ingrese un valor numérico válido\n");
            }
        }
        scanner.close();
    }
}
