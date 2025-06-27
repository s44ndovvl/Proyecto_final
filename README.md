# JUEGO FALTA XP

# Descripción

Falta XP es un rpg, con curva de crecimiento y sistema de combate por turnos. permite explorar la experiencia, a travez de ocpiones, de gestion de objetos, aumento de estadisticas, pelea con enemigos con distintos niveles de dificultad y exploracion de los escenarios conectados en el mapa. Posee descripciones y una historia lineal, junto un jefe final, el cual es nuestro objetivo derrotar para terminar. El juego maneja aspecto del genero rogue like, en el punto de que si mueres tienes que empezar una nueva partida. 


## Cómo compilar y ejecutar

Este sistema ha sido desarrollado en lenguaje C y puede ejecutarse fácilmente utilizando **Visual Studio Code** junto con una extensión para C/C++, como **C/C++ Extension Pack** de Microsoft. Para comenzar a trabajar con el sistema en tu equipo local, sigue estos pasos:

## Requisitos previos:

- Tener instalado [Visual Studio Code](https://code.visualstudio.com/).
- Instalar la extensión **C/C++** (Microsoft).
- Tener instalado un compilador de C (como **gcc**). Si estás en Windows, se recomienda instalar [MinGW](https://www.mingw-w64.org/) o utilizar el entorno [WSL](https://learn.microsoft.com/en-us/windows/wsl/).

## Pasos para compilar y ejecutar:

1) Descarga y descomprime el archivo `.zip` en una carpeta de tu elección.

2) Abre el proyecto en Visual Studio Code
    - Inicia Visual Studio Code.
    - Selecciona `Archivo > Abrir carpeta...` y elige la carpeta donde descomprimiste el proyecto.

3) Compila el código:
    - Abre el archivo principal (por ejemplo, `main.c`).
    - Abre la terminal integrada (`Terminal > Nueva terminal`).
    - En la terminal, compila el programa con el siguiente comando:
        
        gcc *.c -o FaltaXP
       
        
4) Ejecuta el programa
    - Una vez compilado, puedes ejecutar la aplicación con:
        
        ./FaltaXP
        

## Funcionalidades

### Funcionando correctamente:

- Menus pricipal, se muestra correctamente y sus funciones no presentan problemas.
- Sub-menus de exploracion, combate y ayuda, se muestran sin problemas y sus funciones no muestran problemas. Permiten un flujo normal del programa.
- Menu de ayuda, deja una explicacion completa de los elementos que conforman el juego.
- La lectura de escenarios junto con la conexion entre ellos, funciona correctamente.
- Lectura de mobs, ocurre sin problemas.
- Se asignan de manera correcta los mobs segun su dificultad a cada escenario. *
- Se inicializa sin problemas el jugador, al inicializar el juego. Con el nombre seleccionado por el jugador y sus estadisticas y equipaciones iniciales.
- Funcion de exploracion del mapa funciona correctamente, permite el cambio de escenario y entra en batalla en cada cambio.
- Se puede revisar el inventario del jugador satisfactoriamente. *
- Impletacion correcta del flujo del combate por turnos, con sus 3 opciones.
- Se selecciona aleatoriamente un enemigo de los que estan en el escenario.
- Ataque del jugador consigue restar la vida a el enemigo y hacer los ajustes correspondientes.
- Ataque del enemigo consigue restar vida del jugador, manejar las excepciones, como inmunidad y hacer los ajustes sobre la armadura correspondientes.
- Uso de posiciones y sus bufos, aumentan las estadisticas como deberian.
- reinicio de estadisticas luego de batalla. *
- La armadura otorga correctamente defensa y su bufo asociado.
- Si la armadura se rompe, el jugador pierde la defensa y bufos que esta proporciona de forma correcta.
- El enemigo suelta alatoriamente objetos al jugador y le otorga una cantidad de experiencia correctamente. 
- Ocurre bien la subida de estadisticas al llegar a cierta cantidad de experiencia. 
- Se maneja bien el game over, una vez el jugador pierde.
- Se implementa correctamente el win, cuando el jugador gana derrotando al boss final. * 

### Problemas conocidos:

-Problemas con la muestra de texto en consola, sobre todo con las tildes o caracteres que no pertenecen al vocabulario ingles.


### A mejorar:

- Implementar una interfaz de usuario más amigable.
- Correguir problemas y errores a la hora de mostrar el texto.
- Implementar un sistema de guardar y cargar partida guardada.
- Hacer que se pueda iniciar una nueva inmediatamente despues del game over o la victoria del juego. En vez de solo terminar toda la ejecucion del programa
- Implementar niveles de dificultad seleccionables en el juego.
- Agregar multiplayer.
- Agregar batalla con mas de un enemigo a la vez.
- Agregar interfaz de seleccion de multiples mapas, con opciones de cargar mapas propios creados por el jugador.

## Ejemplo de uso

# Paso :
# Paso :
# paso intermedio:
# Paso :
# Paso :
# Paso :
# Paso :
# Paso :
# Paso :
# Paso :
# Contribuciones 

### Joaquin Contreras:

- **Auto-evaluación**: 

### Eduardo Sandoval:

- **Auto-evaluación**: 

### Branron Cáceres:

- **Auto-evaluación**: 

### Josue Huaiquil:

- **Auto-evaluación**: 

