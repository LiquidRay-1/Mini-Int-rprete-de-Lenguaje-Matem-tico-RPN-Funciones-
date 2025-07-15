🧠 Proyecto Avanzado en C: Mini-Intérprete de Lenguaje Matemático (RPN + Funciones)

📋 Enunciado del ejercicio

Diseña e implementa, utilizando únicamente el lenguaje de programación C estándar, un intérprete funcional de un lenguaje matemático minimalista que soporte:

- Expresiones en notación postfija (RPN).
- Asignación de variables.
- Definición de funciones por el usuario con parámetros.
- Evaluación de funciones con su propio entorno de variables (simulación de closures).
- Parsers manuales sin uso de bibliotecas externas, ni funciones como strtol, atoi ni eval.

🧪 Ejemplo de uso esperado

> x = 5 3 +
[set] x = 8

> def cuadrado a a a * end
[def] función 'cuadrado' definida

> cuadrado x
[call] cuadrado = 64

> y = x 2 * 4 /
[set] y = 4

> def suma_doble a b a b + 2 * end
[def] función 'suma_doble' definida

> suma_doble x y
[call] suma_doble = 96

🔧 Requisitos técnicos

- Soporte para operadores +, -, *, /.
- Uso de árboles binarios para representar expresiones matemáticas.
- Manejo de memoria manual (malloc, free).
- Implementación de una tabla de símbolos (entorno de ejecución) usando listas enlazadas.
- Soporte para múltiples variables globales y entornos locales de función.
- Interfaz de línea de comandos interactiva.
- El código debe compilar con gcc sin warnings bajo -Wall.

💡 Objetivo pedagógico

Este ejercicio tiene como objetivo poner a prueba y desarrollar habilidades avanzadas en programación C, incluyendo:

- Gestión dinámica de memoria.
- Diseño de estructuras de datos personalizadas (árboles, tablas de símbolos).
- Simulación de conceptos de alto nivel como closures y entornos léxicos.
- Parsing y evaluación de expresiones matemáticas sin librerías de apoyo.
- Pensamiento algorítmico para implementación de un lenguaje desde cero.

🚀 Nivel de dificultad

Extremo. Este ejercicio está pensado como un reto final para estudiantes o desarrolladores que buscan una experiencia cercana a la creación de lenguajes e intérpretes reales, manteniéndose dentro del paradigma de bajo nivel de C.
