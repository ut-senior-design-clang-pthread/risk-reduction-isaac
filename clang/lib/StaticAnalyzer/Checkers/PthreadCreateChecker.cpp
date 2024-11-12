//===--- PthreadCreateChecker.cpp - Prevent pthreads from escaping analysis ---*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallDescription.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"

using namespace clang;
using namespace ento;

class PthreadCreateChecker : public Checker<check::PostCall> {
  const CallDescription CreateFN{ CDM::CLibrary, {"pthread_create"}, 4 };
public:
  void checkPostCall(const CallEvent &Call, CheckerContext &C) const;
};

void PthreadCreateChecker::checkPostCall(const CallEvent &Call,
                                         CheckerContext &C) const {
  if (!CreateFN.matches(Call))
    return;

  // Get the symbolic value corresponding to the file handle.
  SymbolRef FileDesc = Call.getReturnValue().getAsSymbol();
  if (!FileDesc)
    return;

  llvm::errs() << "Found pthread_create(3) call in " << FileDesc << "\n";
}


void ento::registerPthreadCreateChecker(CheckerManager &mgr) {
  mgr.registerChecker<PthreadCreateChecker>();
}

bool ento::shouldRegisterPthreadCreateChecker(const CheckerManager &mgr) {
  return true;
}