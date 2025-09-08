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
TrigramProfile buildTrigramProfile(const Text &text)
{
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    TrigramProfile profile; // map<string, float>

    for (auto line : text)
    {
        // Eliminar salto de línea tipo '\r'
        if (!line.empty() && line.back() == '\r')
            line = line.substr(0, line.length() - 1);

        // Convertir string UTF-8 a wstring
        wstring unicodeLine = converter.from_bytes(line);

        // Ignorar si la línea tiene menos de 3 caracteres
        if (unicodeLine.length() < 3)
            continue;

        // Extraer trigramas
        for (size_t i = 0; i <= unicodeLine.length() - 3; i++)
        {
            wstring trigramW = unicodeLine.substr(i, 3);

            // Convertir wstring -> string UTF-8
            string trigram = converter.to_bytes(trigramW);

            // Contar ocurrencias
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
void normalizeTrigramProfile(TrigramProfile &trigramProfile)
{
    // 1. Calcular suma de los cuadrados
    double sumSquares = 0.0;
    for (const auto& element : trigramProfile)
    {
        sumSquares += element.second * element.second;
    }

    // Evitar división por cero
    if (sumSquares == 0.0)
        return;

    double norm = std::sqrt(sumSquares);

    // 2. Normalizar cada frecuencia
    for (auto& element : trigramProfile)
    {
        element.second /= static_cast<float>(norm);
    }

    return;
}

/**
 * @brief Calculates the cosine similarity between two trigram profiles
 *
 * @param textProfile The text trigram profile
 * @param languageProfile The language trigram profile
 * @return float The cosine similarity score
 */
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile)
{
    // Your code goes here...

    return 0; // Fill-in result here
}

/**
 * @brief Identifies the language of a text.
 *
 * @param text A Text (vector of lines)
 * @param languages A list of Language objects
 * @return string The language code of the most likely language
 */
string identifyLanguage(const Text &text, LanguageProfiles &languages)
{
    // Your code goes here...

    return ""; // Fill-in result here
}
