#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

struct Estudiante {
    std::string nombre;
    double promedio;
};

bool activarColores() {
#ifdef _WIN32
    HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consola == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD modo = 0;
    return GetConsoleMode(consola, &modo) &&
           SetConsoleMode(consola, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
    return true;
#endif
}

const char* colorPorPromedio(double promedio) {
    if (promedio >= 18.0) {
        return "\033[96m"; // Celeste: excelente
    }
    if (promedio >= 17.0) {
        return "\033[95m"; // Rosado: bueno
    }
    return "\033[92m"; // Verde: regular
}

const char* categoriaPorPromedio(double promedio) {
    if (promedio >= 18.0) {
        return "Excelente";
    }
    if (promedio >= 17.0) {
        return "Bueno";
    }
    return "Regular";
}

bool crearReporte(const std::vector<Estudiante>& estudiantes, int excelentes,
                  int buenos, int regulares, double promedioCurso) {
    std::ofstream archivo("reporte_notas.html");
    if (!archivo) {
        std::cerr << "Error: no se pudo crear reporte_notas.html\n";
        return false;
    }

    const int total = excelentes + buenos + regulares;
    const double anguloExcelente = total > 0 ? 360.0 * excelentes / total : 0.0;
    const double anguloBueno = total > 0 ? 360.0 * (excelentes + buenos) / total : 0.0;

    archivo << R"HTML(<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Estad&iacute;stica de notas</title>
  <style>
    :root {
      color-scheme: light;
      font-family: "Segoe UI", Arial, sans-serif;
      background:
        radial-gradient(ellipse at 8% 12%, #fce8f0 0, transparent 29rem),
        radial-gradient(ellipse at 92% 38%, #e2f5fb 0, transparent 32rem),
        #f5f7fb;
      color: #1f2937;
    }
    * { box-sizing: border-box; }
    body { margin: 0; padding: 32px 18px; }
    main { max-width: 1080px; margin: 0 auto; }
    h1 { margin-bottom: 8px; color: #172554; }
    h2 { color: #172554; }
    .subtitle { margin-top: 0; color: #526176; }
    .hero {
      position: relative;
      display: flex;
      min-height: 220px;
      align-items: flex-start;
      overflow: hidden;
      padding: 28px 32px;
      border: 1px solid #e6eaf2;
      border-radius: 20px;
      background: linear-gradient(115deg, #fff0f5, #effbff 58%, #f1f9ec);
      box-shadow: 0 8px 24px #1e293b0d;
    }
    .hero-copy { position: relative; z-index: 1; max-width: 620px; }
    .kids-art {
      position: absolute;
      right: 8px;
      bottom: 0;
      width: min(48%, 450px);
      height: 170px;
    }
    .course-rating {
      display: flex;
      flex-wrap: wrap;
      align-items: center;
      gap: 8px 14px;
      margin-top: 18px;
      color: #526176;
    }
    .stars {
      position: relative;
      display: inline-block;
      color: #e6eaf0;
      font-size: 1.55rem;
      letter-spacing: 2px;
      line-height: 1;
    }
    .stars-fill {
      position: absolute;
      inset: 0 auto 0 0;
      overflow: hidden;
      color: #f2bd4b;
      white-space: nowrap;
    }
    .rating-value { color: #172554; font-weight: 700; }
    .row-number { color: #64748b; font-variant-numeric: tabular-nums; }
    .panel {
      margin: 22px 0;
      padding: 24px;
      border: 1px solid #e1e8f0;
      border-radius: 16px;
      background: white;
      box-shadow: 0 8px 24px #1e293b0d;
    }
    .bar-row {
      display: grid;
      grid-template-columns: 34px minmax(125px, 180px) minmax(100px, 1fr) 52px 90px;
      align-items: center;
      gap: 14px;
      margin: 15px 0;
    }
    .student { font-weight: 600; }
    .track {
      height: 25px;
      border-radius: 5px;
      background: #edf1f6;
      perspective: 500px;
    }
    .bar {
      position: relative;
      height: 20px;
      min-width: 4px;
      margin-top: 2px;
      border-radius: 4px;
      transform: rotateY(-12deg);
      transform-origin: left center;
      box-shadow: 0 5px 0 var(--side), 0 8px 12px #17255424;
    }
    .bar::before {
      position: absolute;
      top: -5px;
      right: 2px;
      left: 5px;
      height: 5px;
      background: var(--top);
      content: "";
      transform: skewX(-35deg);
      transform-origin: bottom left;
    }
    .excellent { --color: #53c5e3; --side: #2895b3; --top: #8ce2f3; }
    .good { --color: #ed83b8; --side: #c4558d; --top: #f6b2d2; }
    .regular { --color: #65c98a; --side: #389c5e; --top: #9ae2b2; }
    .bar.excellent, .swatch.excellent { background: var(--color); }
    .bar.good, .swatch.good { background: var(--color); }
    .bar.regular, .swatch.regular { background: var(--color); }
    .grade { font-variant-numeric: tabular-nums; text-align: right; }
    .category { color: #526176; font-size: .92rem; }
    .axis {
      display: flex;
      justify-content: space-between;
      margin: 10px 156px 0 242px;
      color: #64748b;
      font-size: .8rem;
    }
    .summary {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 12px;
    }
    .summary-card { padding: 14px; border-radius: 10px; background: #f8fafc; }
    .summary-card strong { display: block; margin-top: 5px; font-size: 1.5rem; }
    .pie-layout {
      display: flex;
      flex-wrap: wrap;
      align-items: center;
      justify-content: center;
      gap: 70px;
      padding: 24px 0 34px;
    }
    .pie {
      position: relative;
      width: 230px;
      aspect-ratio: 1;
      border-radius: 50%;
      background: conic-gradient(
        #53c5e3 0deg )HTML"
           << std::fixed << std::setprecision(2) << anguloExcelente
           << R"HTML(deg, #ed83b8 )HTML"
           << anguloExcelente << "deg " << anguloBueno
           << R"HTML(deg, #65c98a )HTML"
           << anguloBueno << R"HTML(deg 360deg);
      box-shadow: 0 13px 0 #d8e0e9, 0 20px 22px #17255424;
    }
    .pie-label {
      position: absolute;
      top: 50%;
      left: 50%;
      display: grid;
      width: 88px;
      height: 88px;
      place-content: center;
      border-radius: 50%;
      background: white;
      text-align: center;
      transform: translate(-50%, -50%);
      box-shadow: 0 2px 8px #17255420;
    }
    .pie-label strong { font-size: 1.3rem; }
    .legend { display: grid; gap: 14px; }
    .legend-item { display: flex; align-items: center; gap: 10px; }
    .swatch { width: 16px; height: 16px; border-radius: 4px; }
    .note { color: #64748b; font-size: .9rem; }
    @media (max-width: 650px) {
      body { padding: 20px 12px; }
      .panel { padding: 17px; }
      .bar-row { grid-template-columns: 24px 1fr 48px; gap: 7px 10px; }
      .row-number { grid-column: 1; grid-row: 1; }
      .student { grid-column: 2; grid-row: 1; }
      .grade { grid-column: 3; grid-row: 1; }
      .track { grid-column: 1 / -1; grid-row: 2; }
      .category { grid-column: 1 / -1; grid-row: 3; }
      .axis { margin: 8px 0 0; }
      .summary { grid-template-columns: 1fr; }
      .pie-layout { gap: 30px; }
      .hero { min-height: 250px; padding: 22px; }
      .kids-art { right: 0; width: 72%; height: 120px; opacity: .75; }
      .hero-copy { padding-bottom: 95px; }
    }
  </style>
</head>
<body>
  <main>
    <header class="hero">
      <div class="hero-copy">
        <h1>Cuadro estad&iacute;stico de notas</h1>
        <p class="subtitle">Promedios de P1, P2 y P3 sobre 20, ordenados de mayor a menor.</p>
        <div class="course-rating">
          <span>Calificaci&oacute;n general del curso</span>
          <span class="stars" role="img" aria-label=")HTML"
            << std::setprecision(2) << promedioCurso << " de 20; "
            << promedioCurso / 4.0 << R"HTML( de 5 estrellas">
            <span class="stars-fill" style="width: )HTML"
            << std::setprecision(1) << promedioCurso / 20.0 * 100.0
            << R"HTML(%">★★★★★</span>★★★★★
          </span>
          <span class="rating-value">)HTML"
            << std::setprecision(2) << promedioCurso << "/20 · "
            << promedioCurso / 4.0 << R"HTML(/5</span>
        </div>
      </div>
      <svg class="kids-art" viewBox="0 0 450 170" aria-hidden="true">
        <path d="M10 157 Q225 135 440 157 L450 180 L0 180Z" fill="#dff2df"/>
        <circle cx="72" cy="42" r="18" fill="#f3c9a7"/>
        <path d="M54 42 Q57 16 78 23 Q91 27 90 44 Q81 32 70 34 Q62 41 54 42Z" fill="#71564e"/>
        <path d="M45 150 Q48 82 72 76 Q99 83 102 150Z" fill="#a9ddec"/>
        <path d="M55 91 L32 121 M91 92 L113 115" fill="none" stroke="#f3c9a7" stroke-width="10" stroke-linecap="round"/>
        <circle cx="72" cy="48" r="2" fill="#364152"/><circle cx="84" cy="48" r="2" fill="#364152"/>
        <path d="M72 57 Q78 63 85 57" fill="none" stroke="#c56f69" stroke-width="2" stroke-linecap="round"/>
        <circle cx="190" cy="35" r="20" fill="#f0c7a6"/>
        <path d="M170 38 Q169 11 192 14 Q210 16 211 39 Q199 27 184 29Z" fill="#51483e"/>
        <path d="M157 150 Q160 78 190 72 Q222 79 226 150Z" fill="#f4b8d0"/>
        <path d="M168 91 L144 112 M215 92 L239 113" fill="none" stroke="#f0c7a6" stroke-width="10" stroke-linecap="round"/>
        <circle cx="185" cy="42" r="2" fill="#364152"/><circle cx="198" cy="42" r="2" fill="#364152"/>
        <path d="M185 52 Q192 58 200 52" fill="none" stroke="#c56f69" stroke-width="2" stroke-linecap="round"/>
        <circle cx="319" cy="43" r="18" fill="#d9a982"/>
        <path d="M301 43 Q303 20 321 22 Q339 24 338 44 Q326 33 313 35Z" fill="#403a38"/>
        <path d="M289 150 Q291 83 318 77 Q348 82 351 150Z" fill="#cbb8eb"/>
        <path d="M300 93 L277 117 M343 94 L366 115" fill="none" stroke="#d9a982" stroke-width="10" stroke-linecap="round"/>
        <circle cx="313" cy="48" r="2" fill="#364152"/><circle cx="326" cy="48" r="2" fill="#364152"/>
        <path d="M313 57 Q320 63 328 57" fill="none" stroke="#a85e59" stroke-width="2" stroke-linecap="round"/>
        <path d="M390 29 l4 9 10 1-8 6 3 10-9-6-9 6 3-10-8-6 10-1z" fill="#f4d57d" opacity=".9"/>
      </svg>
    </header>
    <section class="panel">
      <h2>Promedio por estudiante — gr&aacute;fico de barras 3D</h2>
      <p class="note">Las barras muestran el promedio en una escala de 0 a 20.</p>
)HTML";

    for (std::size_t i = 0; i < estudiantes.size(); ++i) {
        const Estudiante& estudiante = estudiantes[i];
        const char* clase = estudiante.promedio >= 18.0
                                ? "excellent"
                                : (estudiante.promedio >= 17.0 ? "good" : "regular");
        archivo << "      <div class=\"bar-row\">\n"
                << "        <span class=\"row-number\">" << i + 1 << ".</span>\n"
                << "        <span class=\"student\">" << estudiante.nombre << "</span>\n"
                << "        <div class=\"track\"><div class=\"bar " << clase
                << "\" style=\"width: " << std::setprecision(1)
                << estudiante.promedio * 5.0 << "%\"></div></div>\n"
                << "        <span class=\"grade\">" << std::fixed << std::setprecision(2)
                << estudiante.promedio << "</span>\n"
                << "        <span class=\"category\">" << categoriaPorPromedio(estudiante.promedio)
                << "</span>\n"
                << "      </div>\n";
    }

    archivo << R"HTML(      <div class="axis"><span>0</span><span>5</span><span>10</span><span>15</span><span>20</span></div>
    </section>
    <section class="panel">
      <h2>Distribuci&oacute;n por categor&iacute;a — gr&aacute;fico de pastel 3D</h2>
      <div class="pie-layout">
        <div class="pie" role="img" aria-label="Gr&aacute;fico de pastel con la proporci&oacute;n de excelentes, buenos y regulares">
          <div class="pie-label"><strong>)HTML"
            << total << R"HTML(</strong><span>estudiantes</span></div>
        </div>
        <div class="legend">
          <div class="legend-item"><span class="swatch excellent"></span><span>Excelente (18 a 20): <strong>)HTML"
            << excelentes << " (" << std::setprecision(1)
            << (total > 0 ? 100.0 * excelentes / total : 0.0)
            << R"HTML(%)</strong></span></div>
          <div class="legend-item"><span class="swatch good"></span><span>Bueno (17 a menos de 18): <strong>)HTML"
            << buenos << " (" << (total > 0 ? 100.0 * buenos / total : 0.0)
            << R"HTML(%)</strong></span></div>
          <div class="legend-item"><span class="swatch regular"></span><span>Regular (menos de 17): <strong>)HTML"
            << regulares << " (" << (total > 0 ? 100.0 * regulares / total : 0.0)
            << R"HTML(%)</strong></span></div>
        </div>
      </div>
      <p class="note">Excelente: celeste · Bueno: rosado · Regular: verde</p>
    </section>
  </main>
</body>
</html>
)HTML";

    if (!archivo) {
        std::cerr << "Error: ocurrio un problema al escribir reporte_notas.html\n";
        return false;
    }
    return true;
}

int main() {
    // Notas P1, P2 y P3 tomadas de Libro1.xlsx.
    const std::vector<Estudiante> estudiantes = {
        {"Andy Ayala", (17 + 18 + 19) / 3.0},
        {"Raquel Chimbolema", (19 + 17 + 16) / 3.0},
        {"Yudith Lagla", (18 + 18 + 16) / 3.0},
        {"Camila Ligua", (17 + 18 + 19) / 3.0},
        {"Melany Guaman", (15 + 16 + 18) / 3.0},
        {"Alejandra Guevara", (16 + 17 + 17) / 3.0},
        {"Lizbeth Mayla", (17 + 18 + 18) / 3.0},
        {"Malany Ona", (17 + 16 + 19) / 3.0},
        {"Pamela Obando", (18 + 18 + 19) / 3.0},
        {"Amy Orbes", (17 + 18 + 19) / 3.0},
    };

    std::vector<Estudiante> ordenados = estudiantes;
    std::sort(ordenados.begin(), ordenados.end(),
              [](const Estudiante& a, const Estudiante& b) {
                  if (a.promedio != b.promedio) {
                      return a.promedio > b.promedio;
                  }
                  return a.nombre < b.nombre;
              });

    const bool colores = activarColores();
    const char* reiniciar = colores ? "\033[0m" : "";
    int excelentes = 0;
    int buenos = 0;
    int regulares = 0;
    double sumaPromedios = 0.0;

    std::cout << "CUADRO ESTADISTICO DE NOTAS (promedio sobre 20)\n"
              << "Orden: de mayor a menor\n"
              << "Categorias: Excelente >= 18, Bueno >= 17 y Regular < 17\n\n";

    for (const Estudiante& estudiante : ordenados) {
        const char* color = colores ? colorPorPromedio(estudiante.promedio) : "";
        const int longitudBarra = static_cast<int>(estudiante.promedio + 0.5);
        sumaPromedios += estudiante.promedio;

        if (estudiante.promedio >= 18.0) {
            ++excelentes;
        } else if (estudiante.promedio >= 17.0) {
            ++buenos;
        } else {
            ++regulares;
        }

        std::cout << std::left << std::setw(20) << estudiante.nombre << " "
                  << std::fixed << std::setprecision(2) << std::setw(5)
                  << estudiante.promedio << " "
                  << color << std::string(longitudBarra, '#') << reiniciar
                  << " " << categoriaPorPromedio(estudiante.promedio) << '\n';
    }

    const double promedioCurso =
        ordenados.empty() ? 0.0 : sumaPromedios / ordenados.size();
    if (!crearReporte(ordenados, excelentes, buenos, regulares, promedioCurso)) {
        return 1;
    }

    std::cout << "\nRESUMEN\n"
              << "Excelente: " << excelentes << '\n'
              << "Bueno:     " << buenos << '\n'
              << "Regular:   " << regulares << '\n'
              << "\nLeyenda: "
              << (colores ? colorPorPromedio(18.0) : "") << "Excelente (celeste)"
              << reiniciar << " | "
              << (colores ? colorPorPromedio(17.0) : "") << "Bueno (rosado)"
              << reiniciar << " | "
              << (colores ? colorPorPromedio(16.0) : "") << "Regular (verde)"
              << reiniciar << '\n';
    std::cout << "Se genero el grafico en reporte_notas.html\n";

    return 0;
}
