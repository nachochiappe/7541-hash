#include <stdlib.h>
#include "hash.h"
#include "lista.h"

// Struct de los nodos del hash
struct nodo_hash {
	char *clave;
	void *valor;
};

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
	nodo_hash_t* nodo_hash = NULL;
	if (!(nodo_hash = malloc(sizeof(nodo_hash_t)))) return NULL;
	nodo_hash->clave = clave;
	nodo_hash->valor = dato;

	return nodo_hash;
}

// Devuelve el factor de carga del hash
float factor_de_carga(hash_t *hash) {
	return ((float) (hash->cantidad / hash->tamanio));
}

// Función hash DJB
unsigned int djbhash(const char* clave, size_t tam) {
	unsigned int hash = 5381;
	unsigned int i = 0;

	for(i = 0; i < tam; clave++, i++) {
		hash = ((hash * 32) + hash) + (*clave);
	}

	return hash % tam;
}

// Devuelve el nodo si la clave existe en la lista hash->tabla[pos_vect], NULL si no
nodo_hash_t* clave_existe(hash_t *hash, const char *clave, size_t *pos_vect) {
	if (!hash->tabla[pos_vect]) return NULL;
	for (int i = 0; i < lista_largo(hash->tabla[pos_vect]); i++) {
		nodo_hash_t* nodo = hash->tabla[pos_vect][i];
		// Si la clave del nodo es igual a la pasada por parámetro termino el ciclo
		if (strcmp(nodo->clave, clave) == 0) return nodo;
	}
	return NULL;
}

// Reemplaza el dato de una clave del hash por otro dato pasado
// como parámetro.
bool hash_reemplazar(hash_t *hash, const char *clave, void *dato) {
	size_t pos_vect = djbhash(clave, hash->tamanio);
	nodo_hash_t* nodo = clave_existe(hash, clave, pos_vect);
	if (!nodo) return false;
	if (hash->destruir_dato) hash->destruir_dato(nodo->valor);
	nodo->valor = dato;
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
	if (factor_de_carga(hash) >= (float) 0.7) {
		if (!hash_redimensionar(hash)) return false;
	}
	
	// Obtengo la posición del vector donde guardar el nodo
	size_t pos_vect = djbhash(clave, hash->tamanio);
	
	// Creo una copia de la clave en caso de que la modifiquen desde afuera
	char *clave_copia = strcpy(malloc(strlen(clave) + 1), clave);
	
	// Reemplazo en caso de que la clave pertenezca al hash
	if (hash_reemplazar(hash, clave_copia, dato)) return true;
	
	// Genero un nuevo nodo del hash
	nodo_hash_t* nodo_hash = nodo_hash_crear(clave, dato);
	if (!nodo_hash) return false;
	
	// Inserto el nodo en la lista correspondiente
	lista_insertar_primero(hash->tabla[pos_vect], nodo_hash);
	
	free(clave_copia);
	hash->cantidad++;
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave) {
	
}

void *hash_obtener(const hash_t *hash, const char *clave) {
	
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
	size_t pos_vect = djbhash(clave, hash->tamanio);
	nodo_hash_t* nodo = clave_existe(hash, clave, pos_vect);
	if (!nodo) return false;
	return true;
}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}

void hash_destruir(hash_t *hash) {
	
}

/***********************************
 * FUNCIONES DEL ITERADOR DEL HASH *
 ***********************************/

hash_iter_t *hash_iter_crear(const hash_t *hash) {
	
}

bool hash_iter_avanzar(hash_iter_t *iter) {
	
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	
}

bool hash_iter_al_final(const hash_iter_t *iter) {
	
}

void hash_iter_destruir(hash_iter_t* iter) {
	
}