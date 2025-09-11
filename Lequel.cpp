/**
 * @brief Lequel? language identification based on trigrams
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 *
 * @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a
 */

#include <cmath>
#include <codecvt>
#include <locale>
#include <iostream>
#include <string>
#include "Lequel.h"
#include <map>

using namespace std;

/**
 * @brief Builds a trigram profile from a given text.
 *
 * @param text Vector of lines (Text)
 * @return TrigramProfile The trigram profile
 */
TrigramProfile buildTrigramProfile(const Text& text)
{
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    TrigramProfile profile; // map<string, float>

    // Iterar sobre cada línea del texto
    for (const auto& line : text)
    {
        string processedLine = line;

        // Eliminar salto de línea tipo '\r' si existe
        if (!processedLine.empty() && processedLine.back() == '\r')
            processedLine = processedLine.substr(0, processedLine.length() - 1);

        // Convertir string UTF-8 a wstring para manejar caracteres Unicode
        wstring unicodeLine = converter.from_bytes(processedLine);

        // Ignorar líneas con menos de 3 caracteres
        if (unicodeLine.length() < 3)
            continue;

        // Extraer todos los trigramas de la línea
        for (size_t i = 0; i <= unicodeLine.length() - 3; i++)
        {
            // Obtener trigrama como wstring (3 caracteres Unicode)
            wstring trigramW = unicodeLine.substr(i, 3);

            // Convertir wstring trigrama de vuelta a string UTF-8
            string trigram = converter.to_bytes(trigramW);

            // Incrementar la frecuencia del trigrama en el perfil
            profile[trigram] += 1.0f;
        }
    }

    return profile;


    // Your code goes here...
    /*for (auto line : text)
    {
        if ((line.length() > 0) &&
            (line[line.length() - 1] == '\r'))
            line = line.substr(0, line.length() - 1);
    }
    */
    // Tip: converts UTF-8 string to wstring
    // wstring unicodeString = converter.from_bytes(textLine);

    // Tip: convert wstring to UTF-8 string
    // string trigram = converter.to_bytes(unicodeTrigram);

   // return TrigramProfile(); // Fill-in result here
}

/**
 * @brief Normalizes a trigram profile.
 *
 * @param trigramProfile The trigram profile.
 */
void normalizeTrigramProfile(TrigramProfile& trigramProfile)
{
    // Paso 1: Calcular la suma de los cuadrados de todas las frecuencias
    double sumOfSquares = 0.0;
    for (const auto& element : trigramProfile)
    {
        double frequency = element.second;
        sumOfSquares += frequency * frequency;
    }

    // Verificar si la suma es cero para evitar división por cero
    if (sumOfSquares == 0.0)
        return;

    // Paso 2: Calcular la raíz cuadrada de la suma
    double norm = std::sqrt(sumOfSquares);

    // Paso 3: Dividir cada frecuencia por la norma
    for (auto& element : trigramProfile)
    {
        element.second = element.second / static_cast<float>(norm);
    }
}
/**
 * @brief Calculates the cosine similarity between two trigram profiles
 *
 * @param textProfile The text trigram profile
 * @param languageProfile The language trigram profile
 * @return float The cosine similarity score
 */
float getCosineSimilarity(TrigramProfile& textProfile, TrigramProfile& languageProfile)
{
    float similarity = 0.0f;

    // Iterar sobre cada trigrama del perfil del texto
    for (const auto& element : textProfile)
    {
        const std::string& trigram = element.first;
        float textFrequency = element.second;

        // Buscar si este trigrama también existe en el perfil del lenguaje
        auto it = languageProfile.find(trigram);
        if (it != languageProfile.end())
        {
            // Si existe, obtener su frecuencia en el perfil del lenguaje
            float languageFrequency = it->second;

            // Multiplicar las frecuencias y sumar al total de similitud
            similarity += textFrequency * languageFrequency;
        }
        // Si el trigrama no existe en el perfil del lenguaje, no contribuye a la similitud
        // (equivale a multiplicar por 0)
    }

    return similarity;
}

/**
 * @brief Identifies the language of a text.
 *
 * @param text A Text (vector of lines)
 * @param languages A list of Language objects
 * @return string The language code of the most likely language
 */
string identifyLanguage(const Text& text, LanguageProfiles& languageProfiles)
{
    // Paso 1: Construir el perfil de trigramas del texto
    TrigramProfile textProfile = buildTrigramProfile(text);

    // Paso 2: Normalizar las frecuencias del perfil del texto
    normalizeTrigramProfile(textProfile);

    // Variables para guardar el mejor resultado
    string bestLanguageCode = "";
    float maxSimilarity = -1.0f; // Inicializar con un valor menor que cualquier similitud posible

    // Paso 3: Iterar sobre todos los perfiles de lenguajes
    for (auto& languageProfile : languageProfiles)
    {
        // Calcular la similitud coseno entre el perfil del texto y el perfil del lenguaje
        float similarity = getCosineSimilarity(textProfile, languageProfile.trigramProfile);

        // Verificar si esta similitud es la mayor encontrada hasta ahora
        if (similarity > maxSimilarity)
        {
            maxSimilarity = similarity;
            bestLanguageCode = languageProfile.languageCode;
        }
    }

    // Paso 4: Devolver el código del lenguaje con máxima similitud coseno
    return bestLanguageCode;
}