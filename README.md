# Proyecto semestral: Métricas de centralidad en redes 

## 👥 Integrantes
- Alfonso González
- Matías Cuello
- Marco Liguempi

---

## 💻 Instalación, compilación y ejecución

### 1. Clonar el repositorio

```bash
git clone https://github.com/Yuuzito/Proyecto_semestral_EDD.git
cd Proyecto_semestral_EDD
```

### 2. Compilación y Ejecución

#### 🐧 2.1 Linux

- **Compilación:**

```bash
# Programa principal
g++ main.cpp -o output/main

# Pruebas de rendimiento
g++ main_rendimiento.cpp -o output/main_rendimiento

# Pruebas de experimentos
g++ main_experimentos.cpp -o output/main_experimentos
```

- **Ejecución:**

```bash
./output/main
```

```bash
./output/main_rendimiento
```

```bash
./output/main_experimentos
```

#### 2.2 Windows

- **Compilación:**

```PowerShell
# Programa principal
g++ main.cpp -o output\main.exe

# Pruebas de rendimiento
g++ main_rendimiento.cpp -o output\main_rendimiento.exe

# Pruebas de experimentos
g++ main_experimentos.cpp -o output\main_experimentos.exe
```

- **Ejecución:**

```PowerShell
.\output\main.exe
```

```PowerShell
.\output\main_rendimiento.exe
```

```PowerShell
.\output\main_experimentos.exe
```


## Nota
Se hizo uso de inteligencia artificial (Gemini) como herramienta de apoyo para archivos main, incluyendo diseño de funciones auxiliares y manejo de pruebas. Todo el código ha sido validado, probado y comentado de modo que se mantenga consistente con los requerimientos de nuestro proyecto.