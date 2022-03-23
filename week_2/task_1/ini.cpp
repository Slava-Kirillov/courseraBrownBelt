#include "ini.h"

namespace Ini {

    pair<string_view, string_view> Split(string_view line, char by) {
        size_t pos = line.find(by);
        string_view left = line.substr(0, pos);

        if (pos < line.size() && pos + 1 < line.size()) {
            return {left, line.substr(pos + 1)};
        } else {
            return {left, string_view()};
        }
    }


    Section &Document::AddSection(string name) {
        return sections[move(name)];
    }

    const Section &Document::GetSection(const string &name) const {
        return sections.at(name);
    }

    size_t Document::SectionCount() const {
        return sections.size();
    }

    Document Load(istream &input) {
        Document newDoc;

        Section *last_section_pointer = nullptr;

        for (string line; getline(input, line);) {
            string_view sv(line);

            size_t first = sv.find('[');

            if (first != string_view::npos) {
                size_t second = sv.find(']', first);
                if (second != string_view::npos && second - first > 1) {
                    auto section_name = sv.substr(first + 1, second - 1);
                    last_section_pointer = &newDoc.AddSection({section_name.begin(), section_name.end()});
                    continue;
                }
            }

            pair<string_view, string_view> key_value = Split(line, '=');
            if (!key_value.first.empty() && !key_value.second.empty()) {
                (*last_section_pointer).insert(
                        {
                                string{key_value.first.begin(), key_value.first.end()},
                                string{key_value.second.begin(), key_value.second.end()}
                        }
                );
            }
        }

        return move(newDoc);
    }

}

//
