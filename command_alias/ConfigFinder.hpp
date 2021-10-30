#pragma once
#include <vector>
#include <string>
#include <filesystem>

namespace command_alias {
	using Directory = std::vector<std::filesystem::directory_entry>;

	inline Directory getDirectoryContent(std::string path)
	{
		Directory vec;
		for ( auto it{ std::filesystem::directory_iterator{ std::move(path) } }; !it._At_end(); ++it )
			vec.emplace_back(*it);
		return vec;
	}

	inline Directory getAllFilesWithExtension(std::string path, const std::string& extension)
	{
		Directory vec;

		for ( auto it{ std::filesystem::directory_iterator{ std::move(path) } }; !it._At_end(); ++it )
			if ( it->is_regular_file() )
				if ( const auto path{ it->path() }; path.has_extension() && path.extension() == extension )
					vec.emplace_back(*it);

		return vec;
	}
}