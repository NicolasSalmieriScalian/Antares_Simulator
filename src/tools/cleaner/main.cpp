/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <antares/logs/logs.h>
#include <antares/study/finder.h>
#include <yuni/core/getopt.h>
#include <antares/args/args_to_utf8.h>
#include <antares/utils/utils.h>
#include <antares/study/cleaner.h>
#include <antares/version.h>
#include <antares/sys/policy.h>
#include <antares/locale.h>

using namespace Yuni;
using namespace Antares;

#define SEP Yuni::IO::Separator

static bool onProgress(uint)
{
    return true;
}

class StudyFinderCleaner final : public Data::StudyFinder
{
public:
    StudyFinderCleaner() : listOnly(false), mrproper(false)
    {
    }

    void onStudyFound(const String& folder, Data::Version version) override
    {
        if ((int)version != (int)Data::versionLatest)
        {
            logs.warning() << "requires format upgrade, ignoring " << folder;
            return;
        }

        if (listOnly)
        {
            logs.info() << folder;
            return;
        }

        auto* cleaner = new Data::StudyCleaningInfos(folder);
        cleaner->setCustomExcludeList(exclude);
        cleaner->onProgress.bind(&onProgress);
        if (cleaner->analyze())
            cleaner->performCleanup();
        delete cleaner;

        if (mrproper)
        {
            String path;
            path << folder << SEP << "output";
            IO::Directory::Remove(path);

            path.clear();
            path << folder << SEP << "logs";
            IO::Directory::Remove(path);
        }
    }

public:
    bool listOnly;
    bool mrproper;
    Yuni::String exclude;
}; // class StudyFinderCleaner

int main(int argc, char* argv[])
{
    // locale
    InitializeDefaultLocale();

    Antares::logs.applicationName("cleaner");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
    String::Vector optInput;
    bool optPrintOnly = false;
    bool optMrProper = false;

    Yuni::String optExclude;

    // Command Line options
    {
        // Parser
        GetOpt::Parser options;
        //
        options.addParagraph(String() << "Antares Study Cleaner v" << VersionToCString() << "\n");
        // Input
        options.remainingArguments(optInput);
        // Output
        options.add(optInput, 'i', "input", "An input folder where to look for studies");
        // Exclude some files/folders
        options.add(optExclude,
                    'e',
                    "exclude",
                    "Colon-separated list of excluded files/folders to prevent from destruction. "
                    "Exemple : --exclude fileA:directoryB:fileC");

        // Format
        options.addFlag(optPrintOnly, ' ', "dry", "List the folder only and do nothing");

        options.addFlag(optMrProper, ' ', "mrproper", "Suppress the outputs and logs files");

        // Version
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");

        if (options(argc, argv) == GetOpt::ReturnCode::error)
            return options.errors() ? 1 : 0;

        if (optVersion)
        {
            PrintVersionToStdCout();
            return 0;
        }
    }

    // Load the local policy settings
    LocalPolicy::Open();
    LocalPolicy::CheckRootPrefix(argv[0]);

    if (not optInput.empty())
    {
        StudyFinderCleaner updater;
        updater.listOnly = optPrintOnly;
        updater.mrproper = optMrProper;
        if (!optExclude.empty())
        {
            updater.exclude = optExclude;
        }
        updater.lookup(optInput);
        updater.wait();
    }

    logs.info() << "done.";
    LocalPolicy::Close();
    return 0;
}
