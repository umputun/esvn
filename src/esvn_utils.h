#if !defined(ESVN_UTILS_H)
#define ESVN_UTILS_H

#include <ctime>

#include <qlistview.h> 
#include <qdir.h>
#include <vector>

namespace EsvnLVI
{
    enum Item {
        ITEM_NAME = 0,
        ITEM_FULL_PATH = 16,
        ITEM_WORKING_DIRECTORY = 20,
        ITEM_RELATIVE_PATH = 21,
    };

    QListViewItem* getFirstSelectedItem(QListView *view);
}

// Simple two-component path representation : base and name

class C_Path
{
public:
    C_Path();
    virtual ~C_Path();

public:
    virtual QString Name() const = 0;
    virtual QString Path() const = 0;
    virtual bool isDir() const = 0;

    virtual QString FullPath() const {
        if (Path().isEmpty()) {
            return "";
        } else if (Name().isEmpty()) {
            return Path();
        } else {
            return Path() + "/" + Name();
        }
    }

public:
    static QString Concat(const QString& prefix, const QString& suffix)
    {
        if (prefix.isEmpty()) {
            return suffix;
        } else if (suffix.isEmpty()) {
            return prefix;
        }

        return prefix + "/" + suffix;
    }
};

// Directory path implementation

class C_DirPath : public C_Path
{
public:
    C_DirPath();
    C_DirPath(const QDir& path) {
        operator=(path);
    }
    ~C_DirPath();

public:
    virtual QString Name() const;
    virtual QString Path() const;
    virtual bool isDir() const;

public:
    void SetName(const QString& name);
    void SetPath(const QString& name);
    C_DirPath& operator=(const QDir& path);

private:
    QString name_;
    QString pathname_;
};

// Standalone path object

class C_PathCopy : public C_Path
{
public:
    C_PathCopy();
    C_PathCopy(const C_Path& from);
    ~C_PathCopy();

public:
    C_PathCopy& operator=(const C_Path& from);

public:
    virtual QString Name() const;
    virtual QString Path() const;
    virtual bool isDir() const;

private:
    QString name_;
    QString pathname_;
    bool is_dir_;
};

// File path implementation

class C_RelativePath : public C_Path
{
public:
    C_RelativePath(const C_Path& parent, const QString& name);
    ~C_RelativePath();

public:
    virtual QString Name() const;
    virtual QString Path() const;
    virtual bool isDir() const;

public:
    void SetIsDir(bool is_dir);

private:
//    const C_Path& parent_;
    const C_PathCopy parent_;
    QString name_;
    bool is_dir_;
};

// Selection containers

typedef std::vector<C_PathCopy> PathSelection;

class C_SinglePathSelection
{
public:
    C_SinglePathSelection(const C_Path& selected)
    {
        selection_.push_back(selected);
    }
    operator const PathSelection& () const
    {
        return selection_;
    }

private:
    PathSelection selection_;
};

#endif // ESVN_UTILS_H

