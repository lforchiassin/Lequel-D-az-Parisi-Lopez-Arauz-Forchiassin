/**
 * @brief Lequel? language identification based on trigrams
 * @author Marc S. Ressl, Rocco Diaz Parisi, Jorge Lopez Arauz, Luca Mateo Forchiassin
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

	// Iterates over each line in the text
    for (const auto& line : text)
    {
        string processedLine = line;

		//Deletes carriage return character '\r' if it exists
        if (!processedLine.empty() && processedLine.back() == '\r')
            processedLine = processedLine.substr(0, processedLine.length() - 1);

		// Converts the line to a wide string for proper Unicode handling
        wstring unicodeLine = converter.from_bytes(processedLine);

		// Ignores lines that are too short to contain any trigrams
        if (unicodeLine.length() < 3)
            continue;

		// Extracts trigrams from the line
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

	// Verifies to avoid division by zero
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

		// Search for the trigram in the language profile
        auto it = languageProfile.find(trigram);
        if (it != languageProfile.end())
        {
			// If exists, get its frequency
            float languageFrequency = it->second;

			// Multiply the frequencies and add to the similarity score
            similarity += textFrequency * languageFrequency;
        }
		// If the trigram does not exist in the language profile, it contributes 0 to the similarity
        // (as if multiplying by 0)
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
	// Build trigram profile for the input text
    TrigramProfile textProfile = buildTrigramProfile(text);

    // Normalize the text trigram profile
    normalizeTrigramProfile(textProfile);

    string bestLanguageCode = ""; 
	float maxSimilarity = -1.0f; // Inicialice with -1 to ensure any similarity will be higher

	// Iterate over each language profile to find the best match
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
