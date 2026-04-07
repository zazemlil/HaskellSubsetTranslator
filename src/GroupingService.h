#pragma once

#include "AST.h"

struct GroupedDecls {
    std::vector<std::string> order;
    std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> groups;
};

class GroupingService {
public:
    static GroupingService& getInstance() {
        static GroupingService instance;
        return instance;
    }

    // удаляем копирование
    GroupingService(const GroupingService&) = delete;
    GroupingService& operator=(const GroupingService&) = delete;

    GroupedDecls groupByName(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls) {
        GroupedDecls result;

        for (const auto& decl : decls) {
            if (!decl || decl->getStatementCount() == 0)
                continue;

            auto nameNode = decl->getStatement(0);
            auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);

            if (!id)
                continue;

            const std::string& name = id->getValue();

            if (result.groups.find(name) == result.groups.end()) {
                result.order.push_back(name);
            }

            result.groups[name].push_back(decl);
        }

        return result;
    }

private:
    GroupingService() = default;
};
