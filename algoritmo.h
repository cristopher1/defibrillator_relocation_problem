#ifndef HILL_CLIMBING_MEJOR_MEJORA
#define HILL_CLIMBING_MEJOR_MEJORA

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <utility>
#include <set>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <algorithm>

enum class Parametro
{
  nombre_archivo = 1,
  numero_restart,
  semilla
};

typedef std::pair<long, long> coordenadas;
typedef std::vector<unsigned long> posicion;
typedef void (*enfoque)(const int n_eventos, const int radio, const float presupuesto,
                        const int n_restart, std::vector<std::set<coordenadas>> coberturas);
typedef float (*costo)(const posicion aeds_iniciales, const posicion solucion_candidata);
typedef posicion (*generar_solucion)(const int largo, unsigned long presupuesto, unsigned int n_aeds);
typedef void (*imprimir)(const posicion aeds_iniciales, const posicion mejor_solucion,
                         const posicion coords_x, const posicion coords_y);

/*Representa el resultado de la Cobertura Total*/
class ResultadoCT
{
private:
  unsigned long cobertura;
  std::set<coordenadas> eventosCubiertos;

public:
  ResultadoCT();
  ResultadoCT(const unsigned long cobertura, const std::set<coordenadas> eventosCubiertos);
  unsigned long getCobertura();
  std::set<coordenadas> getEventosCubiertos();
};

/*Representa el resultado de la Función de Evaluación*/
class ResultadoFEv
{
private:
  float costo;
  unsigned long cobertura;
  std::set<coordenadas> eventosCubiertos;

public:
  ResultadoFEv();
  ResultadoFEv(const float costo, const unsigned long cobertura,
               const std::set<coordenadas> eventosCubiertos);
  float getCosto();
  unsigned long getCobertura();
  std::set<coordenadas> getEventosCubiertos();
};

/*Representa el resultado de Hill Climbing con Mejor Mejora*/
class ResultadoHCMM
{
private:
  posicion resultado;
  float costo;
  unsigned long cobertura;

public:
  ResultadoHCMM();
  ResultadoHCMM(const posicion resultado, const float costo,
                const unsigned long cobertura);
  posicion getResultado();
  float getCosto();
  unsigned long getCobertura();
};

/*Carga las coordenadas y los aeds existentes de un archivo de texto.*/
std::tuple<float, unsigned int, unsigned int, unsigned int, const char *, posicion, posicion, posicion> cargar_datos(const char *nombre_archivo);

/*Establece la semilla para replicar el experimento.*/
void establecer_semilla(long semilla);

/*
 * Genera un vector de conjuntos, donde cada posición del vector representa el conjunto de todos aquellos
 * eventos OHCA que se encuentran a una distancia radio del evento OHCA actual (representado por la
 * posición del vector).
 */
std::vector<std::set<coordenadas>> obtener_coberturas(const posicion coords_x,
                                            const posicion coords_y,
                                            const unsigned int radio);

/*Generador de soluciones iniciales para problemas fijos*/
posicion generar_si_fijo(const int largo, unsigned long presupuesto,
                         unsigned int n_aeds);

/*Generador de soluciones iniciales para problemas flexibles*/
posicion generar_si_flexible(const int largo, unsigned long presupuesto,
                             unsigned int n_aeds);

/*Permite calcular la cobertura total de los eventos OHCA cubiertos por los AEDs*/
ResultadoCT cobertura_total_inicial(std::vector<std::set<coordenadas>> coberturas,
                                    posicion solucion_candidata);

/*Permite calcular la cobertura total de las soluciones candidatas*/
ResultadoCT cobertura_total(const std::vector<std::set<coordenadas>> coberturas,
                            const std::set<coordenadas> eventos_cubiertos,
                            const posicion solucion_candidata,
                            const unsigned int posicion, const bool agregado);

/*Calcular costo cobertura fijo*/
float ccc_fijo(const posicion aeds_iniciales, const posicion solucion_candidata);

/*Calcular costo cobertura flexible*/

float ccc_flexible(const posicion aeds_iniciales, const posicion solucion_candidata);

/*Función de evaluación.*/
ResultadoFEv f_ev(const std::vector<std::set<coordenadas>> coberturas,
                  const posicion aeds_iniciales,
                  const posicion solucion_candidata,
                  const std::set<coordenadas> eventos_cubiertos,
                  const unsigned int posicion, const bool agregado,
                  costo const cost, const unsigned long presupuesto);

/*Hill climbing con mejor mejora.*/
ResultadoHCMM hc_mm(std::vector<std::set<coordenadas>> coberturas, posicion aeds_iniciales,
                    posicion solucion_inicial, const int radio,
                    const float presupuesto, costo cost);

/*Imprime la información asociada a problemas de tipo fijo*/
void info_aeds_fijo(const posicion aeds_iniciales, const posicion mejor_solucion,
                    const posicion coords_x, const posicion coords_y);

/*Imprime la información asociada a problemas de tipo flexible*/
void info_aeds_flexible(const posicion aeds_iniciales, const posicion mejor_solucion,
                        const posicion coords_x, const posicion coords_y);

/*Función utilizada para resolver el problema con los diferentes enfoques.*/
void resolver(const int n_eventos, const int radio, const float presupuesto,
              const int n_restart, const unsigned int n_aeds_iniciales,
              const posicion aeds_iniciales, const posicion coords_x,
              const posicion coords_y, costo const cost,
              generar_solucion const solucion,
              imprimir const info);

#endif
