#include "algoritmo.h"

/*implementacion de los metodos de la clase ResultadoFEv*/

ResultadoFEv::ResultadoFEv()
{
  this->costo = 0.0;
  this->cobertura = 0;
  this->eventosCubiertos = std::set<coordenadas>();
}

ResultadoFEv::ResultadoFEv(const float costo, const unsigned long cobertura,
                           const std::set<coordenadas> eventosCubiertos)
{
  this->costo = costo;
  this->cobertura = cobertura;
  this->eventosCubiertos = eventosCubiertos;
}

float ResultadoFEv::getCosto()
{
  return this->costo;
}

unsigned long ResultadoFEv::getCobertura()
{
  return this->cobertura;
}

std::set<coordenadas> ResultadoFEv::getEventosCubiertos()
{
  return this->eventosCubiertos;
}

/*implementacion de los metodos de la clase ResultadoHCMM*/

ResultadoHCMM::ResultadoHCMM()
{
  this->resultado = posicion();
  this->costo = 0.0;
  this->cobertura = 0;
}

ResultadoHCMM::ResultadoHCMM(const posicion resultado, const float costo, const unsigned long cobertura)
{
  this->resultado = resultado;
  this->costo = costo;
  this->cobertura = cobertura;
}

posicion ResultadoHCMM::getResultado()
{
  return this->resultado;
}

float ResultadoHCMM::getCosto()
{
  return this->costo;
}

unsigned long ResultadoHCMM::getCobertura()
{
  return this->cobertura;
}

/**
 * Se hace una redirección entre descriptores de archivo. Se usa el nombre del archivo
 * para obtener un descriptor de dicho archivo. Luego el descriptor original (entregado
 * en la variable descriptor), es reemplazado por el descriptor de archivo mencionado
 * anteriormente. ¿Para que se usa?, se puede usar para reemplazar el descriptor
 * STDIN por el descriptor de archivo obtenido por nombre_archivo, y de esta
 * manera usar cin para leer el contenido del archivo, en lugar de hacer las operaciones
 * que se realizan al utilizar open o fopen.
 */
int redirigir_descriptor(const int descriptor, const char *const nombre_archivo, const int modo)
{
  int descriptor_copiado, descriptor_nuevo;

  if ((descriptor_copiado = dup(descriptor)) == -1 ||
      (descriptor_nuevo = open(nombre_archivo, modo)) == -1 ||
      dup2(descriptor_nuevo, descriptor) == -1 ||
      close(descriptor_nuevo) == -1)
  {
    perror("Error: ");
    exit(-1);
  }

  /**
   * Este descriptor contiene el contenido del descriptor original (variable descriptor).
   * Usado para restaurar el valor del descriptor original.
   */
  return descriptor_copiado;
}

/**
 * Restaura el descriptor de archivo original.
 */
void restaurar_descriptor(const int descriptor_copiado, const int descriptor_modificado)
{
  /*redirigir el descriptor de entrada para que reciba la entrada estandar*/
  if (dup2(descriptor_copiado, descriptor_modificado) == -1 || close(descriptor_copiado) == -1)
  {
    perror("Error: ");
    exit(-1);
  }
}

std::tuple<float, unsigned int, unsigned int, unsigned int, const char *,
           posicion, posicion, posicion>
cargar_datos(const char *nombre_archivo)
{
  float presupuesto;
  unsigned int n_eventos, radio, n_aeds_iniciales;
  unsigned long coord_x, coord_y, aed_inicial;
  posicion coords_x, coords_y, aeds_iniciales;
  int descriptor_original;

  descriptor_original = redirigir_descriptor(STDIN_FILENO, nombre_archivo, O_RDONLY);

  std::cin >> n_eventos >> presupuesto >> radio;
  for (unsigned int c = 0; c < n_eventos; ++c)
  {
    std::cin >> coord_x >> coord_y >> aed_inicial;
    coords_x.push_back(coord_x), coords_y.push_back(coord_y), aeds_iniciales.push_back(aed_inicial);
    n_aeds_iniciales += aed_inicial;
  }

  restaurar_descriptor(descriptor_original, STDIN_FILENO);

  const char *enfoque = n_aeds_iniciales ? "flexible" : "fijo";

  return std::make_tuple(presupuesto, radio, n_eventos, n_aeds_iniciales,
                         enfoque, coords_x, coords_y, aeds_iniciales);
}

void establecer_semilla(long semilla)
{
  srand(semilla);
}

posicion generar_solucion_inicial_enfoque_fijo(const int largo,
                                               unsigned long presupuesto, unsigned int n_aeds)
{
  // Solución inicial, al comienzo no tiene aeds.
  posicion solucion(largo, 0);
  /*
   * Se genera una solución inicial con una cantidad de AEDs que varia desde presupuesto hasta
   * presupuesto + 40%, esto con el fin de hacer variar la cantidad de AEDs de cada solución
   * inicial, además se ha elegido que la cantidad de AEDs iniciales varie en esos rangos,
   * ya que en general la cantidad de AEDs que poseen las soluciones factibles están bastante cerca de
   * la cantidad de presupuesto con el cual se cuenta.
   */
  presupuesto = presupuesto * (1 + ((rand() % 4) / 10));
  for (auto aed = solucion.begin(), ultimo_aed = solucion.end(); aed != ultimo_aed && presupuesto > 0; ++aed)
  {
    /*
     * Se genera un 0 o 1 de forma aleatoria, que indica si existe o no un aed y se agrega
     * a la solución inicial.
     */
    unsigned long hay_aed = ((rand() % 101) / 100) >= 0.9;
    if (hay_aed)
    {
      *aed = 1;
      --presupuesto;
    }
  }
  return solucion;
}

posicion generar_solucion_inicial_enfoque_flexible(const int largo,
                                                   unsigned long presupuesto, unsigned int n_aeds)
{
  // Solución inicial, al comienzo no tiene aeds.
  posicion solucion(largo, 0);
  /*
   * Se genera una solución inicial con la cantidad de aeds que se encuentran ubicados inicialmente
   * en las ubicaciones de OHCA dadas. Debido a que existe una cantidad inicial de AEDs que se
   * deben colocar en la solución, se decide aumentar la probabilidad de generar 1's.
   */
  for (auto aed = solucion.begin(), ultimo_aed = solucion.end(); aed != ultimo_aed && n_aeds > 0; ++aed)
  {
    /*
     * Se genera un 0 o 1 de forma aleatoria, que indica si existe o no un aed y se agrega
     * a la solución inicial.
     */
    unsigned long hay_aed = ((rand() % 101) / 100) >= 0.7;
    if (hay_aed)
    {
      *aed = 1;
      --n_aeds;
    }
  }
  return solucion;
}

/*Calcula la cobetura total de la solución inicial*/
std::pair<int, std::set<coordenadas>> obtener_cobertura_total_inicial(std::vector<std::set<coordenadas>> coberturas,
                                                                      posicion solucion_candidata)
{
  std::set<coordenadas> eventos_cubiertos;

  auto cobertura = coberturas.begin(), ultima_cobertura = coberturas.end();
  auto aed = solucion_candidata.begin(), ultimo_aed = solucion_candidata.end();

  for (; cobertura != ultima_cobertura && aed != ultimo_aed; ++cobertura, ++aed)
  {
    /*
     * Cada vez que un AED se encuentra cubriendo algún evento OHCA, se obtienen todas las posiciones
     * de eventos OHCA que se encuentran a una distancia radio de él, que a su vez serán posiciones
     * cubiertas por el AED.
     */
    if (*aed)
    {
      eventos_cubiertos.insert((*cobertura).begin(), (*cobertura).end());
    }
  }
  return std::make_pair(eventos_cubiertos.size(), eventos_cubiertos);
}

std::pair<int, std::set<coordenadas>> obtener_cobertura_total(const std::vector<std::set<coordenadas>> coberturas,
                                                              const std::set<coordenadas> eventos_cubiertos,
                                                              const posicion solucion_candidata,
                                                              const unsigned int posicion_aed,
                                                              const bool movimiento_realizado)
{
  const bool aed_agregado = movimiento_realizado;
  std::set<coordenadas> nuevos_eventos_cubiertos = eventos_cubiertos;
  std::set<coordenadas> cobertura_aed = coberturas[posicion_aed];

  /*
   * Debido a que el movimiento consiste en quitar o agregar un solo AED a la vez, entonces
   * cuando se agrega un AED en una posición, se agrega a la cobertura de OHCA
   * actual los eventos OHCA cubiertos por el AED agregado y en caso de quitar un AED,
   * se quitan de los eventos cubiertos, los eventos OHCA que ese AED cubria.
   */

  if (aed_agregado)
  {
    nuevos_eventos_cubiertos.insert(cobertura_aed.begin(), ++cobertura_aed.end());
    return std::make_pair(nuevos_eventos_cubiertos.size(), nuevos_eventos_cubiertos);
  }

  for (auto evento = cobertura_aed.begin(), ultimo_evento = cobertura_aed.end(); evento != ultimo_evento; ++evento)
  {
    nuevos_eventos_cubiertos.erase(*evento);
  }
  return std::make_pair(nuevos_eventos_cubiertos.size(), nuevos_eventos_cubiertos);
}

float calcular_costo_cobertura_enfoque_fijo(const posicion aeds_iniciales, const posicion solucion_candidata)
{
  return accumulate(solucion_candidata.begin(), solucion_candidata.end(), 0);
}

float calcular_costo_cobertura_enfoque_flexible(const posicion aeds_iniciales, const posicion solucion_candidata)
{
  long numero_aeds_iniciales, numero_aeds_solucion_candidata, aeds_agregados, aeds_movidos;

  numero_aeds_iniciales = accumulate(aeds_iniciales.begin(), aeds_iniciales.end(), 0);
  numero_aeds_solucion_candidata = accumulate(solucion_candidata.begin(), solucion_candidata.end(), 0);
  aeds_agregados = numero_aeds_solucion_candidata - numero_aeds_iniciales;
  aeds_agregados = aeds_agregados > 0 ? aeds_agregados : 0;
  aeds_movidos = 0;

  for (auto aed_inicial = aeds_iniciales.begin(), ultimo_aed_inicial = aeds_iniciales.end(),
            aed_solucion_candidata = solucion_candidata.begin(),
            ultimo_aed_solucion_candidata = solucion_candidata.end();
       aed_inicial != ultimo_aed_inicial && aed_solucion_candidata != ultimo_aed_solucion_candidata;
       ++aed_inicial, ++aed_solucion_candidata)
  {
    /*
     * Si un aed estaba inicialmente en una ubicación pero en la solución candidata no aparece
     * en dicha posición, entonces el aed se ha movido.
     */
    if ((*aed_inicial) && !(*aed_solucion_candidata))
    {
      aeds_movidos++;
    }
  }

  return aeds_agregados + (0.2 * aeds_movidos);
}

ResultadoFEv funcion_evaluacion(const std::vector<std::set<coordenadas>> coberturas,
                                const posicion aeds_iniciales, const posicion solucion_candidata,
                                const std::set<coordenadas> eventos_cubiertos,
                                const unsigned int posicion_aed, const bool movimiento_realizado,
                                costo const calcular_costo, const unsigned long presupuesto)
{
  float costo;
  unsigned long cobertura;
  std::set<coordenadas> eventos_cubiertos_actuales;

  costo = calcular_costo(aeds_iniciales, solucion_candidata);
  std::tie(cobertura, eventos_cubiertos_actuales) = obtener_cobertura_total(coberturas, eventos_cubiertos,
                                                                            solucion_candidata, posicion_aed,
                                                                            movimiento_realizado);
  return ResultadoFEv(costo, cobertura, eventos_cubiertos_actuales);
}

/*
 * Genera un vector, donde cada posición representa una determinada ubicación de un
 * evento OHCA y en cada una de esas posiciones se almacena el conjunto de todas aquellas
 * ubicaciones de eventos OHCAs que se encuentran a una distancia radio de él. De esta
 * manera, es posible almacenar la cobertura de un punto de OHCA y realizando uniones
 * entre los conjuntos de OHCA de cada posición del vector se puede obtener la cobertura
 * total de las soluciones entregadas por HC con MM.
 */
std::vector<std::set<coordenadas>> obtener_coberturas(const posicion coords_x,
                                                      const posicion coords_y,
                                                      const unsigned int radio)
{
  std::vector<std::set<coordenadas>> coberturas;

  const unsigned long radio_cobertura_aed = static_cast<unsigned long>(radio);

  for (auto coord_x_evento_actual = coords_x.begin(), ultima_coord_x_evento_actual = coords_x.end(),
            coord_y_evento_actual = coords_y.begin(), ultima_coord_y_evento_actual = coords_y.end();
       coord_x_evento_actual != ultima_coord_x_evento_actual &&
       coord_y_evento_actual != ultima_coord_y_evento_actual;
       ++coord_x_evento_actual, ++coord_y_evento_actual)
  {

    /*
     * Eventos OHCA dentro del mismo radio de covertura del AED. Si se coloca un AED en este sitio,
     * puede cubrir todos los eventos OHCA dentro de este conjunto.
     */
    std::set<coordenadas> eventos_cubiertos;

    for (auto coord_x_otro_evento = coords_x.begin(), ultima_coord_x_otro_evento = coords_x.end(),
              coord_y_otro_evento = coords_y.begin(), ultima_coord_y_otro_evento = coords_y.end();
         coord_x_otro_evento != ultima_coord_x_otro_evento &&
         coord_y_otro_evento != ultima_coord_y_otro_evento;
         ++coord_x_otro_evento, ++coord_y_otro_evento)
    {

      unsigned long distancia_entre_eventos = sqrt(pow(*coord_x_evento_actual - *coord_x_otro_evento, 2) +
                                                   pow(*coord_y_evento_actual - *coord_y_otro_evento, 2));
      if (distancia_entre_eventos <= radio_cobertura_aed)
      {
        eventos_cubiertos.insert(std::make_pair(*coord_x_otro_evento, *coord_y_otro_evento));
      }
    }
    coberturas.push_back(eventos_cubiertos);
  }
  return coberturas;
}

void imprimir_resultado_enfoque_fijo(const posicion aeds_iniciales, const posicion mejor_solucion,
                                     const posicion coords_x, const posicion coords_y)
{
  long numero_aeds_agregados = accumulate(mejor_solucion.begin(), mejor_solucion.end(), 0);

  std::cout << "Cantidad de AEDs agregados: " << numero_aeds_agregados << std::endl;
  std::cout << "Posiciones de los AEDs: " << std::endl;

  for (unsigned int ubicacion = 0; ubicacion < mejor_solucion.size(); ++ubicacion)
  {
    // Se revisa si hay un aed en dicha ubicación
    if (mejor_solucion[ubicacion])
    {
      std::cout << "coordenada X: " << coords_x[ubicacion] << " coordenada Y: " << coords_y[ubicacion] << std::endl;
    }
  }
}

void imprimir_resultado_enfoque_flexible(const posicion aeds_iniciales, const posicion mejor_solucion,
                                         const posicion coords_x, const posicion coords_y)
{
  long numero_aeds_mejor_solucion, numero_aeds_iniciales, numero_aeds_agregados, numero_aeds_movidos;

  numero_aeds_mejor_solucion = accumulate(mejor_solucion.begin(), mejor_solucion.end(), 0);
  numero_aeds_iniciales = accumulate(aeds_iniciales.begin(), aeds_iniciales.end(), 0);
  numero_aeds_agregados = numero_aeds_mejor_solucion - numero_aeds_iniciales;
  numero_aeds_agregados = numero_aeds_agregados > 0 ? numero_aeds_agregados : 0;
  numero_aeds_movidos = 0;

  for (unsigned int ubicacion = 0; ubicacion < aeds_iniciales.size(); ++ubicacion)
  {
    /*
     * Si hay un AED en aeds_iniciales, pero no hay AED en mejor_solucion
     * entonces ese AED se ha sido movido.
     */
    if (aeds_iniciales[ubicacion] && !mejor_solucion[ubicacion])
    {
      ++numero_aeds_movidos;
    }
  }

  std::cout << "Cantidad de AEDs agregados: " << numero_aeds_agregados << std::endl;
  std::cout << "Cantidad de AEDs movidos: " << numero_aeds_movidos << std::endl;

  for (unsigned int ubicacion = 0; ubicacion < mejor_solucion.size(); ++ubicacion)
  {
    if (mejor_solucion[ubicacion])
    {
      std::cout << "coordenada X: " << coords_x[ubicacion] << " coordenada Y: " << coords_y[ubicacion] << std::endl;
    }
  }
}

ResultadoHCMM hill_climbing_mejor_mejora(std::vector<std::set<coordenadas>> coberturas,
                                         posicion aeds_iniciales, posicion solucion_inicial,
                                         const int radio, const float presupuesto,
                                         costo const calcular_costo)
{
  /*
   * Variables terminadas en _act sirven para procesar datos de la mejor solución (actual)
   * que se utilizan para generar los vecinos mediante el movimiento elegido y obtener
   * sus coberturas.
   * Variables mejor_solucion que no terminan en _act y variables mejor se utilizan para
   * guardar la información asociada a la mejor solución encontrada al realizar la explotación.
   * Variable solucion_candidata representa un vecino.
   */
  float mejor_costo, mejor_costo_actual;
  unsigned long mejor_cobertura, mejor_cobertura_actual;
  std::set<coordenadas> mejor_eventos_cubiertos_actual, mejor_eventos_cubiertos;
  posicion mejor_solucion, solucion_candidata;
  bool movimiento_realizado, hay_mejor_solucion;

  mejor_costo = calcular_costo(aeds_iniciales, solucion_inicial);
  mejor_costo_actual = mejor_costo;
  std::tie(mejor_cobertura, mejor_eventos_cubiertos_actual) = obtener_cobertura_total_inicial(coberturas, solucion_inicial);
  mejor_cobertura_actual = mejor_cobertura;
  mejor_eventos_cubiertos = mejor_eventos_cubiertos_actual;
  mejor_solucion = solucion_inicial;
  solucion_candidata = mejor_solucion;

  do
  {
    /*Al inicio del proceso no hay una mejor solución*/
    hay_mejor_solucion = false;
    for (unsigned int posicion_aed = 0; posicion_aed < mejor_solucion.size(); ++posicion_aed)
    {
      /*
       * El movimiento consiste en quitar o agregar un aed en una determinada posición de un
       * evento OHCA, si no hay un aed (solucion_candidata 0) se agrega 1 (solucion_candidata 1).
       * En caso contrario, se hace lo opuesto.
       */
      solucion_candidata[posicion_aed] = solucion_candidata[posicion_aed] ? 0 : 1;
      movimiento_realizado = solucion_candidata[posicion_aed];
      ResultadoFEv calidad = funcion_evaluacion(coberturas, aeds_iniciales, solucion_candidata,
                                                mejor_eventos_cubiertos_actual, posicion_aed,
                                                movimiento_realizado, calcular_costo,
                                                presupuesto);
      /*
       * Se van a aceptar soluciones no factibles en caso de que el costo de la mejor solución
       * sea mayor al presupuesto, en caso contrario, solo se encontrarán soluciones factibles.
       * Por lo tanto se tienen los siguientes dos casos para considerar una solución como mejor:
       *
       * 1. Si el costo de la mejor solución es mayor al presupuesto y el costo de la nueva solución
       * es menor al costo de la mejor solución entonces se considera la nueva solución como
       * mejor solución (en este caso se aceptan soluciones factibles y no factibles, se busca bajar
       * el costo de las soluciones para que calcen con el presupuesto).
       *
       * 2. Si el costo de la nueva solución es menor o igual al presupuesto y tiene mejor cobertura
       * que la mejor solución entonces se vuelve mejor solución (solo se aceptan soluciones factibles,
       * se busca aumentar la cobertura manteniendo un costo dentro del presupuesto).
       */
      float costo_actual = calidad.getCosto();
      unsigned long cobertura_actual = calidad.getCobertura();
      if (((mejor_costo_actual > presupuesto) && (costo_actual < mejor_costo_actual)) ||
          ((costo_actual <= presupuesto) && (cobertura_actual > mejor_cobertura_actual)))
      {
        mejor_solucion = solucion_candidata;
        mejor_costo = costo_actual;
        /*
         * Cuando la solución encontrada salga del presupuesto, entonces dicha solución no tendrá
         * cobertura, ya que es una solución no factible, en caso contrario se calcula su cobertura
         * total.
         */
        mejor_cobertura = mejor_costo > presupuesto ? 0 : cobertura_actual;
        mejor_eventos_cubiertos = calidad.getEventosCubiertos();
        hay_mejor_solucion = true;
      }
      /*
       * Se aplica nuevamente el movimiento, para que el vector vuelva a su estado original.
       */
      solucion_candidata[posicion_aed] = solucion_candidata[posicion_aed] ? 0 : 1;
    }
    mejor_costo_actual = mejor_costo;
    mejor_cobertura_actual = mejor_cobertura;
    solucion_candidata = mejor_solucion;
    mejor_eventos_cubiertos_actual = mejor_eventos_cubiertos;
  } while (hay_mejor_solucion);

  return ResultadoHCMM(mejor_solucion, mejor_costo, mejor_cobertura);
}

void resolver(const int n_eventos, const int radio, const float presupuesto,
              const int n_restart, const unsigned int n_aeds_iniciales,
              const posicion aeds_iniciales, const posicion coords_x,
              const posicion coords_y, costo const calcular_costo,
              generar_solucion const obtener_solucion_inicial,
              imprimir const imprimir_resultado)
{

  ResultadoHCMM mejor;
  std::vector<std::set<coordenadas>> coberturas = obtener_coberturas(coords_x, coords_y, radio);
  for (int c = 0; c < n_restart; ++c)
  {
    posicion solucion_inicial = obtener_solucion_inicial(n_eventos, presupuesto, n_aeds_iniciales);
    ResultadoHCMM nuevo = hill_climbing_mejor_mejora(coberturas, aeds_iniciales, solucion_inicial,
                                                     radio, presupuesto, calcular_costo);
    unsigned long mejor_cobertura = mejor.getCobertura(), nuevo_cobertura = nuevo.getCobertura();
    float mejor_costo = mejor.getCosto(), nuevo_costo = nuevo.getCosto();
    if ((nuevo_cobertura > mejor_cobertura) || ((nuevo_cobertura == mejor_cobertura) && (nuevo_costo < mejor_costo)))
    {
      mejor = nuevo;
    }
  }
  std::cout << "Cobertura: " << mejor.getCobertura() << std::endl;
  std::cout << "Porcentaje de cobertura: " << std::fixed << std::setprecision(2) << (mejor.getCobertura() / (float)n_eventos) * 100 << "%" << std::endl;
  std::cout << "Presupuesto sobrante: " << presupuesto - mejor.getCosto() << std::endl;
  imprimir_resultado(aeds_iniciales, mejor.getResultado(), coords_x, coords_y);
}
