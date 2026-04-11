#include "StaticAnalyzer.h"


void StaticAnalyzer::analyze(std::shared_ptr<syntax_tree::ASTNode> root) {
    if (!root) return;
    analyzeNode(root);
}

IRGenerator *StaticAnalyzer::getIRGenerator() {
    return generator;
}

StaticAnalyzer::StaticAnalyzer() {
    generator = new IRGenerator();
}

StaticAnalyzer::~StaticAnalyzer() {
    delete generator;
}

void StaticAnalyzer::analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node) {
    if (!node) return;

    if (node->getNodeType() == "DEFINITIONS") {
        auto& decls = node->getStatements();
        checkContiguity(decls);

        auto grouped = GroupingService::getInstance().groupByName(decls);

        for (const auto& name : grouped.order)
        {
            auto& gdecls = grouped.groups[name];

            checkArity(gdecls);
            checkPatternRedundancy(name, gdecls);
        }
    }

    if (node->getNodeType() == "LET") {
        auto& decls = node->getStatement(0)->getStatements();

        checkContiguity(decls);

        auto grouped = GroupingService::getInstance().groupByName(decls);

        for (const auto& name : grouped.order)
        {
            auto& gdecls = grouped.groups[name];

            checkArity(gdecls);
            checkPatternRedundancy(name, gdecls);
        }
    }

    if (node->getNodeType() == "LC_LET") {
        auto& decls = node->getStatements();

        checkContiguity(decls);

        auto grouped = GroupingService::getInstance().groupByName(decls);

        for (const auto& name : grouped.order)
        {
            auto& gdecls = grouped.groups[name];

            checkArity(gdecls);
            checkPatternRedundancy(name, gdecls);
        }
    }

    if (node->getNodeType() == "WHERE") {
        auto& decls = node->getStatement(1)->getStatements();

        checkContiguity(decls);

        auto grouped = GroupingService::getInstance().groupByName(decls);

        for (const auto& name : grouped.order)
        {
            auto& gdecls = grouped.groups[name];

            checkArity(gdecls);
            checkPatternRedundancy(name, gdecls);
        }
    }

    if (node->getNodeType() == "LIST_COMPREHENSION") {
        for (auto& q : node->getStatement(1)->getStatements()) {
            if (q->getNodeType() == "<-") {
                auto pattern = q->getStatement(0);
                if (!isPattern(pattern)) {
                    throw std::runtime_error("Invalid pattern in generator.");
                }
            }
        }
    }

    for (auto& child : node->getStatements()) {
        analyzeNode(child);
    }

    if (node->getNodeType() == "DEFINITIONS" || node->getNodeType() == "LET" || node->getNodeType() == "WHERE" || node->getNodeType() == "LC_LET") {
        generator->generate(node);
    }
}

void StaticAnalyzer::checkContiguity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::unordered_map<std::string, int> lastPos;

    for (size_t i = 0; i < decls.size(); ++i) {
        if (decls[i]->getNodeType() == "SIGNATURE") continue;

        auto nameNode = decls[i]->getStatement(0);
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);

        if (!id) continue;

        std::string name = id->getValue();

        if (lastPos.count(name) && lastPos[name] != i - 1) {
            throw std::runtime_error(
                "Non-contiguous definitions for function '" + name + "'."
            );
        }

        lastPos[name] = i;
    }
}

size_t StaticAnalyzer::getLambdaAbstractionArity(const std::shared_ptr<syntax_tree::ASTNode>& node) {
    if (node->getStatement(1)->getNodeType() == "λ") {
        return 1 + getLambdaAbstractionArity(node->getStatement(1));
    }
    return 0;
}

bool StaticAnalyzer::isPattern(const std::shared_ptr<syntax_tree::ASTNode> &node) {
    if (!node) return false;

    const std::string type = node->getNodeType();

    if (type == "Identifier")
        return true;

    if (type == "_")
        return true;

    if (type == "LiteralInt" ||
        type == "LiteralFloat" ||
        type == "LiteralString" ||
        type == "LiteralChar")
        return true;

    if (type == "CONSTRUCTOR" || type == "CONSTRUCTOR_PATTERN") {
        for (auto& child : node->getStatements()) {
            if (!isPattern(child))
                return false;
        }
        return true;
    }

    if (type == "TUPLE" || type == "TUPLE_PATTERN") {
        for (auto& child : node->getStatements()) {
            if (!isPattern(child))
                return false;
        }
        return true;
    }

    if (type == "LIST_NODE" || type == "LIST_PATTERN") {
        for (auto& child : node->getStatements()) {
            if (!isPattern(child))
                return false;
        }
        return true;
    }

    if (type == ":") {
        if (node->getStatementCount() != 2)
            return false;

        return isPattern(node->getStatement(0)) &&
               isPattern(node->getStatement(1));
    }

    return false;
}

void StaticAnalyzer::checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    if (decls.empty())
        return;

    int expectedArity = 0, signatureArity = -1;

    for (auto& n : decls) {
        if (n->getNodeType() == "SIGNATURE") {
            signatureArity = n->getStatement(1)->getStatementCount()-1;
            break;
        }
    }
    for (auto& n : decls) {
        if (n->getNodeType() == "DEF" || n->getNodeType() == "=") {
            auto id = n->getStatement(0);
            expectedArity = id->getStatementCount();
            break;
        }
    }

    int signaturesCount = 0;
    for (const auto& decl : decls)
    {
        int arity = -1;

        if (decl->getNodeType() == "SIGNATURE") {
            if (!signaturesCount) {
                signatureArity = decl->getStatement(1)->getStatementCount()-1;
                signaturesCount++;
            }
            else {
                auto nameNode = decls[0]->getStatement(0);
                auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
                throw std::runtime_error(
                    "Multiple definitions of '" + id->getValue() + "' signature."
                );
            }
            
        } else if (decl->getNodeType() == "DEF" || decl->getNodeType() == "=") {
            auto id = decl->getStatement(0);
            arity = id->getStatementCount();
        }

        if (arity != -1 && arity != expectedArity)
        {
            auto nameNode = decl->getStatement(0);
            auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
            throw std::runtime_error(
                "Multiple definitions of '" + id->getValue() + "'."
            );
        }
    }

    if (expectedArity == 0 && decls.size() > 1+signaturesCount) {
        auto nameNode = decls[0]->getStatement(0);
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
        throw std::runtime_error(
            "Multiple definitions of '" + id->getValue() + "' constant."
        );
    }

    if (signatureArity != -1) {
        for (auto& n : decls) {
            if (n->getNodeType() == "DEF" || n->getNodeType() == "=") {
                if (expectedArity+getLambdaAbstractionArity(n) != signatureArity) {
                    auto nameNode = n->getStatement(0);
                    auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
                    throw std::runtime_error(
                        "Arity mismatch in function '" + id->getValue() + "'."
                    );
                }
            }
        }
    }

    if (signaturesCount == 1 && decls.size() == 1) {
        auto nameNode = decls[0]->getStatement(0);
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
        throw std::runtime_error(
            "The type signature for '" + id->getValue() + "' lacks an accompanying binding."
        );
    }
}

PatternKind StaticAnalyzer::getPatternKind(std::shared_ptr<syntax_tree::ASTNode> p) {
    std::string t = p->getNodeType();

    if (t == "_") 
        return WILDCARD;
    
    if (t == "LiteralInt" || t == "LiteralFloat" || t == "LiteralString" || t == "LiteralBoolean")
        return LITERAL;

    if (t == "CONSTRUCTOR_PATTERN")
        return CONSTRUCTOR;

    if (t == "NIL")
        return LIST_EMPTY;

    if (t == "LIST_PATTERN")
        return LIST_ENUM;

    if (t == "LIST_HEAD_TAIL_PATTERN")
        return LIST_CONS;

    return IDENTIFIER;
}

std::vector<std::shared_ptr<syntax_tree::ASTNode>> 
    StaticAnalyzer::extractPatterns(std::shared_ptr<syntax_tree::ASTNode> decl) 
{
    auto& stmts = decl->getStatement(0)->getStatements();
    return stmts;
}

bool StaticAnalyzer::patternCovers(std::shared_ptr<syntax_tree::ASTNode> p1, std::shared_ptr<syntax_tree::ASTNode> p2) {
    auto k1 = getPatternKind(p1);
    auto k2 = getPatternKind(p2);

    if (k1 == WILDCARD || k1 == IDENTIFIER)
        return true;

    if (k1 == LITERAL && k2 == LITERAL) {

        std::ostringstream a,b;
        p1->printValue(a);
        p2->printValue(b);

        return a.str() == b.str();
    }

    if (k1 == CONSTRUCTOR && k2 == CONSTRUCTOR) {

        std::ostringstream a,b;
        p1->getStatement(0)->printValue(a);
        p2->getStatement(0)->printValue(b);

        if (a.str() != b.str())
            return false;

        if (p1->getStatementCount() != p2->getStatementCount()) {
            return false;
        }
        else if (p1->getStatementCount() < 2) {
            return true;
        }

        for (size_t i=1;i<p1->getStatementCount();i++)
            if (!patternCovers(p1->getStatement(i), p2->getStatement(i)))
                return false;

        return true;
    }

    if (k1 == LIST_EMPTY && k2 == LIST_EMPTY)
        return true;

    if (k1 == LIST_CONS && k2 == LIST_CONS)
        return true;

    if (k1 == LIST_ENUM && k2 == LIST_ENUM) {

        auto a = p1->getStatements();
        auto b = p2->getStatements();

        if (a.size() != b.size())
            return false;

        for (size_t i=0;i<a.size();i++)
            if (!patternCovers(a[i], b[i]))
                return false;

        return true;
    }

    return false;
}

bool StaticAnalyzer::rowCovers(
    const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r1,
    const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r2)
{
    if (r1.size() != r2.size())
        return false;

    for (size_t i = 0; i < r1.size(); i++) {

        if (!patternCovers(r1[i], r2[i]))
            return false;
    }

    return true;
}

void StaticAnalyzer::checkPatternRedundancy(std::string name, const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::vector<std::vector<std::shared_ptr<syntax_tree::ASTNode>>> seenRows;

    for (auto& d : decls) {
        if (d->getStatement(0)->getStatementCount() == 0) continue;

        auto row = extractPatterns(d);

        bool redundant = false;
        for (auto& prev : seenRows) {

            if (rowCovers(prev, row)) {
                redundant = true;
                break;
            }
        }

        if (redundant) {

            throw std::runtime_error(
                "Redundant pattern in function definition '" + name + "'.");
        }

        seenRows.push_back(row);
    }
}