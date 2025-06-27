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
- Se asignan de manera correcta los mobs segun su dificultad a cada escenario. 
- Se inicializa sin problemas el jugador, al inicializar el juego. Con el nombre seleccionado por el jugador y sus estadisticas y equipaciones iniciales.
- Funcion de exploracion del mapa funciona correctamente, permite el cambio de escenario y entra en batalla en cada cambio.
- Se puede revisar el inventario y estado del jugador satisfactoriamente. 
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
- Se implementa correctamente el win, cuando el jugador gana derrotando al boss final. 
- Se libera memoria de forma conciente.

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

# Paso 2: Opcion de ayuda

Se despliegan las opciones del menu pricipal: "1) INICIAR NUEVA PARTIDA 2) AYUDA 3) CREDITOS 4) SALIR ", y se selecciona la de ayuda, para poder entender mejor como se interacciona con el juego.

Opción seleccionada: 2) AYUDA

Se despliegan 4 opciones, 3 son mini tutoriales del funcionamiento de los enemigos y la otra para volver al menu principal. Se busca saber mas sobre la exploracion.

Opción seleccionada: 1) SOBRE LOS MOVIMIENTOS

se muestra el mensaje "Para avanzar en tu aventura, selecciona la opcion (EXPLORAR ZONAS) desde el menu de juego. Dependiendo de las zonas disponibles, podras moverte por el mapa utilizando las teclas:    
                W - Arriba    A - Izquierda    S - Abajo    D - Derecha 
Cada zona oculta secretos, enemigos o tesoros. Piensa bien cada paso... en Umbrahal, el peligro acecha en cada rincon" y se vuelve al menu de ayuda.

Opción seleccionada: 2) SOBRE LOS ENEMIGOS

se muestra el mensaje "En las profundidades de Umbrahal, te enfrentaras a criaturas del inframundo que buscan detenerte. Estos enemigos varian en dificultad: FACIL, MEDIO, DIFICIL y el temible BOSS.
A medida que avances por los pisos de la masmorra, los enemigos se volveran mas agresivos y letales. Cada encuentro será un reto, y solo los más preparados sobrevivirán. Los enemigos aparecen de forma aleatoria, y su dificultad dependera del area en la que te encuentres.Derrotarlos no solo sera necesario para sobrevivir, tambien puede recompensarte con items valiosos que podrian cambiar el curso de tu destino." y se vuelve al menu de ayuda.

Opción seleccionada: 3) SOBRE EL EQUIPAMENTO

se muestra el mensaje "En tu descenso por la Masmorra de Umbrahal, encontraras items esenciales que marcaran la diferencia entre la vida y la muerte. Las armaduras te otorgan proteccion contra los ataques del inframundo, reduciendo el daño recibido. Las pociones restauran salud o brindan efectos temporales como inmunidad o estamina. Explora bien cada escenario. El equipamiento correcto puede ser la clave para sobrevivir... o caer." y se vuelve al menu de ayuda.

Opción seleccionada: 4) VOLVER AL MENU PRINCIPAL

se vuelve a las opciones del menu principal

# Paso 2: Iniciar partida

Una vez se vuelve al menu principal, y con mas contexto sobre como funciona las mecanicas del juego se busca iniciar partida.

Opción seleccionada: 1) INICIAR NUEVA PARTIDA

INDIQUE EL NOMBRE DEL NUEVO JUGADOR: edu

se inicia una nueva partida, creando e inicializando al jugador edu. Se muestran las opciones del menu de exploracion: "1) EXPLORAR ZONAS 2) VER ESTADO DEL JUGADOR 3) ATACAR A UN ENEMIGO 4) SALIR AL MENU PRINCIPAL"

# paso 3: Entrar en combate

Se busca aumentar experiencia y recolectar objeto atacando un enemigo.

Opción seleccionada: 3) ATACAR A UN ENEMIGO

aparece un mensaje con las estaditicas del jugador y el enemigo a enfrentarse: "Enemigo: Murcielago Ciego | Vida: 13 | Defensa: 0 | Dificultad : Facil". Luego las opciones de del menu de pelea: 1) ATACAR AL ENEMIGO 2) ATAQUE ESPECIAL 3) USAR POCION 4) HUIR DE LA PELEA.

# Paso 4: Ataque especial / Ataque al enemigo

Para hacerle daño al enemigo puedes elegir entre ataque especial, o el ataque normal. La diferencia entre ambos, es que el ataque especial hace el doble de daño que el ataque normal, pero consume 5 de estamina al jugador, mientras que el ataque normal va recuperando 1 de estamina cada vez que se efectua.

Opción seleccionada: 2) ATAQUE ESPECIAL

El jugador pierde 5 de estamina, y despues del ataque del enemigo pierde cierta cantidad de vida. Se vuelve al menu de pelea.

# Paso 5: Intentar huir.

Si no ve el escenario del combate muy a su favor, puede intentar huir. Aunque con menos estadisticas de vida y estamina este, más le costara esta accion.

Opción seleccionada: 4) HUIR DE LA PELEA.

El enemigo, no te dejo huir de la pelea y te dio un ataque, que te resto más vida. 

# Paso 6: Usar un consumible.

El jugador perdio algo de vida asi que busca recuperarla usando algun consumible.

Opción seleccionada: 3) USAR POCION

=== POCIONES DISPONIBLES ===
1) Pocion de Vida - Vida (+20)
2) Cancelar
Seleccione la opcion a usar: 

Opción seleccionada: 1) Pocion de Vida - Vida (+20)

Utilizas la pocion de vida, y recuperas 20 de vida, luego vuelves al menu de pelea.

# Paso 7: Recolectar items.

Una vez que el enemigo es derrotada este suelta de manera aleatoria una serie de items, que se mostrara en un mini menu de recoleccion: 
"1) [ARMA] Daga de Eco - Ataque: 6 - Durabilidad: 8 
 2)[ARMADURA] Casco de Alas Defensa: 2 - Durabilidad: 5
 3)[ARMADURA] Pechera de Cuero Viejo Defensa: 2 - Durabilidad: 4
 4)TOMAR TODO
 5)CANCELAR"

Opción seleccionada: 1) [ARMA] Daga de Eco - Ataque: 6 - Durabilidad: 8 

Arma actual: Sin arma (Ataque: 0, Durabilidad: 0)
Nueva arma:  Daga de Eco (Ataque: 6, Durabilidad: 8)
Deseas reemplazar tu arma actual? (1 = SI, 0 = NO):

Opción seleccionada: 1 = SI

Luego se equipara esta arma en tu inventario y podras recoger mas o decidir salir al menu exploración

# Paso 8: Ver estado

Despues de la batalla deseas ver en que estado quedo tu personajes.

Opción seleccionada: 2) VER ESTADO DEL JUGADOR

Jugador: edu | Vida: 98 | Estamina: 18 | Ataque: 6 | Defensa: 15 | Nivel: 0
Te encuentras en: Entrada al Abismo

[POCIONES]:
1) Pocion de Vida | Efecto: Vida | Valor: 20

Vuelves al menu de exploracion y seleccionas tu proxima opcion.

# Paso 9: Cambiar de escenario

Una vez superada la primera batalla se decide cambiar de escenario en el menu de exploracion.

Opción seleccionada: 1) EXPLORAR ZONAS

Cuando se cambia de escenario, se llama a batalla inmediatamente y luego si esa batalla sale satisfactoria se indica el nombre del nuevo escenario y su leyenda: "Te has movido a: Pasillo de los Susurros

# Paso 10: Creditos

Luego de cierto tiempo jugando puedes decidir salir del juego desde el menu de exploracion.

Opción seleccionada: 4) SALIR AL MENU PRINCIPAL

Luego querer ver los creditos, antes de terminar tu experiencia

Opción seleccionada: 3) CREDITOS

se muestra: 
"Falta Creatividad Studios Presenta: 
Falta XP

Director: Eduardo Sandoval
Productor: Joaquin Contreras
Gestor de Calidad: Brandon Caceres
Responsable de Integracion: Josue Huaiquil
Coordinador: Eduardo Sandoval 
Creacion de Enemigos: Brandon Caceres
Interface Designer: Josue Huaiquil

Equipo de Programacion:
Eduardo Sandoval
Joaquin Contreras
Brandon Caceres
Josue Huaiquil

Gracias por jugar Falta XP!"

Luego puedes volver al menu principal

# Paso 11: Salir del juego

Cuando ya deseas acabar la expiercia total, simplemente seleccionas salir del manu del menu principal y se acaba la ejecucion del programa.

Opción seleccionada: 4) SALIR

Se acaba la ejecucion del programa

# Contribuciones 

### Joaquin Contreras:
- Creo funcion de movimiento por sala.
- Balanceo de dificultad.
- Implemento csv inicial del mapa y ajusto csv enemigo.
- Creo el liberar memoria y el win.
- Funcion subir de nivel.
- Leer mobs.
- Correccion de funciones generales.

- **Auto-evaluación**: Excelente y optima.

### Eduardo Sandoval:
- Leer escenarios y crear conexiones.
- Contribucion al ciclo de pelea.
- Creo el ataque enemigo.
- Creo funcion huida.
- Ajuste al leer mob.
- Creo estructuras.
- Hizo aplicar estadisticas durante pelea.
- Creacion y escritura del readme.

- **Auto-evaluación**: Excelente y optima.

### Branron Cáceres:
- Creo recolectar objetos y guardar objeto.
- Creo seleccion enemigo.
- Creo asignar enemigo a mapa.
- Creo el crear player.
- creo el usar posiciones.
- Aplicar bufo.
- creo Realizar ataque del jugador.
- Creo reiniciar armadura.
- Hizo el ataque especial.
- gran contribucion en revision de errores.
- Orden general de modularizacion.

- **Auto-evaluación**: Excelente y optima.

### Josue Huaiquil:
- Implemento menus.
- Implemento switches del juego.
- Implemento la ayuda y los creditos.
- Creo el ver estado jugador.
- Hizo gran aporte en la funcion de ramas.
- Gran aporte en la correccion de errores.
- Creo los mostrar map y mob, para hacer revision de las lecturas.
- Implemento la funcion esperar dentro de los extra.
- Comentacion y documentacion del codigo.

- **Auto-evaluación**: Excelente y optima

