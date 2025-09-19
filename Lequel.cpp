/**
 * @brief Lequel? language identification based on trigrams
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 *
 * @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a
 *
 *
 * 25.03 - Algoritmos y Estructuras de Datos
 * Level 2: lequel
 * Integrantes: Rocco Diaz Parisi, Jorge Lopez Arauz, Luca Mateo Forchiassin
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
            wstring trigramW = unicodeLine.substr(i, 3);

            string trigram = converter.to_bytes(trigramW);

            profile[trigram] += 1.0f;
        }
    }

    return profile;
}

/**
 * @brief Normalizes a trigram profile.
 *
 * @param trigramProfile The trigram profile.
 */
void normalizeTrigramProfile(TrigramProfile& trigramProfile)
{
    double sumOfSquares = 0.0;
    for (const auto& element : trigramProfile)
    {
        double frequency = element.second;
        sumOfSquares += frequency * frequency;
    }

    // Verificar si la suma es cero para evitar división por cero
    if (sumOfSquares == 0.0)
        return;

    double norm = std::sqrt(sumOfSquares);

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

    string bestLanguageCode = "";
    float maxSimilarity = -1.0f; // Inicializar con un valor menor que cualquier similitud posible

    // Paso 3: Iterar sobre todos los perfiles de lenguajes
    for (auto& languageProfile : languageProfiles)
    {
        float similarity = getCosineSimilarity(textProfile, languageProfile.trigramProfile);

        if (similarity > maxSimilarity)
        {
            maxSimilarity = similarity;
            bestLanguageCode = languageProfile.languageCode;
        }
    }

    return bestLanguageCode;

}
