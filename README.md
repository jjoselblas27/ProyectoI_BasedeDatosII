# ProyectoI_BasedeDatosII
## INTEGRANTES:
- FERNANDO ADRIANO CHOQQUE MEJIA
- CARLOS FLORES PANDURO
- JUAN LEANDRO BLAS


# DATASET
Utilizamos un dataset - 10+ M. Beatport Tracks / Spotify Audio Features.



- acousticness [0, 1] - A confidence measure from 0.0 to 1.0 of whether the track is acoustic. 1.0 represents high confidence the track is acoustic.

- danceability [0, 1] - Danceability describes how suitable a track is for dancing based on a combination of musical elements including tempo, rhythm stability, beat strength, and overall regularity. A value of 0.0 is least danceable and 1.0 is most danceable.

- duration_ms - The duration of the track in milliseconds.

- energy [0, 1] - Energy is a measure from 0.0 to 1.0 and represents a perceptual measure of intensity and activity. Typically, energetic tracks feel fast, loud, and noisy. For example, death metal has high energy, while a Bach prelude scores low on the scale. Perceptual features contributing to this attribute include dynamic range, perceived loudness, timbre, onset rate, and general entropy.

https://www.kaggle.com/datasets/mcfurland/10-m-beatport-tracks-spotify-audio-features


# ISAM

![ISAM] (./imagenes/isam.png)

## archivos utilizados
nivel 1 :

    root


nivel 2:

    indexPAGE1        indexPAGE2  ..
nivel 3 :

    indexPAge1   indexPAge2   .......


estructura heapfile:

    Page1   Page2   Page3   ---- PageK | Pagek+1  PageK+2 ...
Las paginas desde Page 1 a k estan ordenadas



estructura del datafile (freelist LIFO):

            header
            record1     next1
            record2     next2
            record3     next3
            -----


## busqueda
se realiza una busqueda en el archivo de indices para obtener la posicion de la pagina en la posicion.Posteiormente realizo una busqueda por cada bloque para encontrar todas las ocurrencias de la llave y las almaceno en un vector. Posteriormente accedo al dataFile en las posiciones que ya he hallado.


## insertar
Primero realizo una insercion en el archivo de datos(Datafile), tomando en cuenta que manejo un freelist tipo LIFO.
Despues realizo una insercion en los indices.
como ya esta construido la estructura con la funcion buildIsam() encuentro la posicion de la pagina y itero hasta encontrar una pagina donde pueda insertar, si no encuentro creo una nueva pagina y lo conecto con la ultima pagina.

## delete
Primero realizo una eliminacion en el archivo de datos(dataFile). freelist
Despues realizo una eliminacion en los indices. 
realizo la busqueda de la llave en los indices. y itero sobre las paginas overflow si encuentro similitudes, chanco el valor del ultimo elemento de la pagina y reduzco el contador en 1 de la pagina.


## range Search
Realizo una busqueda del inf y sup. Debido a su construccion los primero (M+1)³ paginas estan ordenadas, entonces como tengo las posicion de la pagina inf y sup, empiezo el pagina inf y recorro todas sus paginas overflow encontrando valores posibles, cuando termino me muevo a la siguiente pagina y realizo lo mismo hasta que llege a la pagina en la posicion sup.




# STATIC HASHING
![STATIC HASH] ("/imagenes/hashStatic.png")


## archivos
 estructura del bucketFIle con freelist tipo LIFO. 

    header
    record
    record
    ......
    record
    --------- fin de los M primeras paginas
    record next
    record next
    ......
    record next


estructura del dataFile con freelist tipo LIFO

    header
    record next
    record next
    ......
    record next

## search
utilizo la funcion hash para ubicar la posicion en el bucketFile. Luego recorro todos los overflow para buscar todas las keys iguales y almaceno las posiciones que tiene esas keys.
Despues abro el archivo datafile y retorno los records en las posiciones guardadas.

## insert

Primero inserto en el dataFile con freelist TIPO LIFO y ubico su posicion en el dataFile.

Utilizo la funcion hash para ubicar la posicion. Luego si puedo insertar en la "main" bucket inserto y termino si no puedo verifico con el siguiente, si tampoco se puede insertar  o no existe siguiente creo una nueva pagina utilizando la posicion del header para verificar si hay espacio vacio, Luego lo concateno con la "main" bucket y hago que el next del "main" bucket sea el next del nuevo bloque.

## delete
Primero realizo obtengo el hash y realizo una busqueda por todos los overflow, si encuentro coincidencia "elimino" el par, es decir, chanco con el par al final de la pagina y reduzco el contador a -1 ademas de guardar la posicion para eliminar en el datafile despues. si detecto que al final de la iteracion sobe la pagina el count es 0 elimino esa pagina a menos que sea la pagina principal. eliminar significa que el padre apunta al next del nodo actual, y marco el nodo actual como eliminado.

Despues elimino los records en el datafile usando la tecnica del freelist LIFO.

## range search

Realizo una busqueda lineal sobre todos los datos

# AVL

![avl] (./imagenes/avl.jpg)

## Search

## insert

## rangeSearch




## comentarios finales

- Una manera de mejorar el ISAM seria aplicar la misma tecnica que en el hash Static para el bucketfile. ya que si se eliminan datos pueden quedar muchos DataPages vacios, menguando la velocidad en las busquedas.

- Lamentablemente no se pudo realizar el parser pero dejare la idea que teniamos:
    - recorrer la query y sacar la informacion relevante para ese pequeño conjunto de querys disponibles y guardarlo en la clase Instruction, posteriormente usar esa funcion instruction para identificar el tipo de operacion. si es una operacion de escritura creo el indice y guardaria en un archivo 3 cosas ("nombre tabla" "index usado" "nombre columna"). De modo que cuando ejecute una funcion de search, eliminacion y/o insercion como se el nombre de la tabla puedo realizar una busqueda linear sobre los indices para saber que indice uso y sobre que columna esta aplicado. de este modo me permite que los indices perduren. 


