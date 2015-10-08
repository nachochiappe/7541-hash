#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "lista.h"
#include <stdio.h>

#define TAM_INICIAL 5

// Struct de los nodos del hash
typedef struct nodo_hash {
	char *clave;
	void *valor;
} nodo_hash_t;

struct hash {
	lista_t** tabla;
	size_t cantidad;
	size_t tamanio;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter{
	const hash_t *hash; 
	size_t pos_vect;
	lista_iter_t *iter_lista;
};

/***********************************
 *        FUNCIONES AUXILIARES     *
 ***********************************/

// Crea un nuevo nodo del hash
nodo_hash_t* nodo_hash_crear(char *clave, void *dato) {
    nodo_hash_t* nodo_hash = malloc(sizeof(nodo_hash_t));
	if (!nodo_hash) return NULL;
	nodo_hash->clave = clave;
	nodo_hash->valor = dato;
	return nodo_hash;
}

// Devuelve el factor de carga del hash
float factor_de_carga(hash_t *hash) {
	return ((float) (hash->cantidad / hash->tamanio));
}

//
unsigned long int fhash(const char* clave, size_t tam) {
	unsigned long int hash = 0;
	long int i = 0;
	while (clave[i] != '\0') {
		hash += ((unsigned long int) clave[i] + 13);
		i++;
	}
	return hash%tam;
}

// Devuelve el nodo si la clave existe en la lista hash->tabla[pos_vect], NULL si no
nodo_hash_t* nodo_en_lista(const hash_t *hash, const char *clave, size_t *pos_vect) {
	lista_iter_t* iter = lista_iter_crear(hash->tabla[*pos_vect]);
    nodo_hash_t* nodo;
    bool pertenece = false;
	while (!lista_iter_al_final(iter)) {
        nodo = lista_iter_ver_actual(iter);
		// Si la clave del nodo es igual a la pasada por parámetro termino el ciclo
		if (strcmp(nodo->clave, clave) == 0){ 
            pertenece = true;
			break;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	if (pertenece) return nodo;
	return NULL;
}

// Reemplaza el dato de una clave del hash por otro dato pasado
// como parámetro. En caso de no encontrar la clave, devuelve false.
bool hash_reemplazar(hash_t *hash, const char *clave, void *dato) {
	size_t pos_vect = fhash(clave, hash->tamanio);
	nodo_hash_t* nodo = nodo_en_lista(hash, clave, &pos_vect);
	if (!nodo) return false;
	if (hash->destruir_dato) hash->destruir_dato(nodo->valor);
	nodo->valor = dato;
	return true;
}

bool hash_redimensionar(hash_t* hash) {
	// Elijo un nuevo tamaño igual a 5 veces el tamaño anterior
	size_t nuevo_tamanio = hash->tamanio * 5;
	lista_t** nueva_tabla = calloc(nuevo_tamanio, sizeof(lista_t*));
	if (!nueva_tabla) return false;
	
	// Creo una nueva lista por cada posición de la nueva tabla
	for (unsigned int i = 0; i < nuevo_tamanio; i++) {
		nueva_tabla[i] = lista_crear();
	}
	// Saco los nodos del hash anterior, los vuelvo a hashear y los inserto
	// en el nuevo hash
	for (unsigned int i = 0; i < hash->tamanio; i++){
		while (!lista_esta_vacia(hash->tabla[i])){
			nodo_hash_t* nodo = lista_borrar_primero(hash->tabla[i]);
			size_t pos_vect = fhash(nodo->clave, nuevo_tamanio);
			lista_insertar_primero(nueva_tabla[pos_vect], nodo);
		}
		// Destruyo las listas del hash anterior
		lista_destruir(hash->tabla[i], NULL);
	}
	
	free(hash->tabla);
	hash->tabla = nueva_tabla;
	hash->tamanio = nuevo_tamanio;
	return true;
}

/***********************************
 *        FUNCIONES DEL HASH       *
 ***********************************/
 
hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;
	
	// Genero una tabla inicializada con ceros
	lista_t** tabla = calloc(TAM_INICIAL, sizeof(lista_t*));
	if (!tabla) {
		free(hash);
		return NULL;
	}
	
	// Genero una lista por cada posición de la tabla
	for (unsigned int i = 0; i < TAM_INICIAL; i++){
		tabla[i] = lista_crear();
	}
	hash->tabla = tabla;
	hash->destruir_dato = destruir_dato;
	hash->cantidad = 0;
	hash->tamanio = TAM_INICIAL;
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	// Redimensiono el hash en caso de que el factor de carga sea >= 70%
	if (factor_de_carga(hash) >= 0.7) {
		if (!hash_redimensionar(hash)) return false;
	}
	// Obtengo la posición del vector donde guardar el nodo
	size_t pos_vect = fhash(clave, hash->tamanio);
	// Creo una copia de la clave en caso de que la modifiquen desde afuera
	char *clave_copia = strcpy(malloc(strlen(clave) + 1), clave);
	// Reemplazo en caso de que la clave pertenezca al hash
	if (hash_reemplazar(hash, clave_copia, dato)) {
		free(clave_copia);
		return true;
	}
	// Genero un nuevo nodo del hash
	nodo_hash_t* nodo = nodo_hash_crear(clave_copia, dato);
	if (!nodo) return false;
	
	// Inserto el nodo en la lista correspondiente
	lista_insertar_primero(hash->tabla[pos_vect], nodo);
	hash->cantidad++;
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave) {
	size_t pos_vect = fhash(clave, hash->tamanio);
	void* dato = NULL;
	lista_iter_t* iter = lista_iter_crear(hash->tabla[pos_vect]);
	while (!lista_iter_al_final(iter)) {
		nodo_hash_t* nodo_actual = lista_iter_ver_actual(iter);
		if (strcmp(nodo_actual->clave, clave) == 0) {
			lista_borrar(hash->tabla[pos_vect], iter);
			dato = nodo_actual->valor;
			free(nodo_actual->clave);
			free(nodo_actual);
			hash->cantidad--;
			break;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave) {
	size_t pos_vect = fhash(clave, hash->tamanio);
	void* dato = NULL;
	nodo_hash_t* nodo = nodo_en_lista(hash, clave, &pos_vect);
	if (!nodo) return NULL;
	dato = nodo->valor;
	return dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
	size_t pos_vect = fhash(clave, hash->tamanio);
	nodo_hash_t* nodo = nodo_en_lista(hash, clave, &pos_vect);
	return nodo != NULL;
}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}


void hash_destruir(hash_t *hash) {
	size_t largo = hash->tamanio;
	for (int i = 0; i < largo; i++) {
		while (!lista_esta_vacia(hash->tabla[i])){
			nodo_hash_t* nodo = lista_borrar_primero(hash->tabla[i]);
			if (hash->destruir_dato != NULL)
				hash->destruir_dato(nodo->valor);
			free(nodo->clave);
			free(nodo);
		}
		lista_destruir(hash->tabla[i], NULL);
	}
	free(hash->tabla);
	free(hash);
}

/***********************************
 * FUNCIONES DEL ITERADOR DEL HASH *
 ***********************************/

hash_iter_t *hash_iter_crear(const hash_t *hash);
	


bool hash_iter_avanzar(hash_iter_t *iter);
	


const char *hash_iter_ver_actual(const hash_iter_t *iter);
	


bool hash_iter_al_final(const hash_iter_t *iter);
	


void hash_iter_destruir(hash_iter_t* iter);
	

