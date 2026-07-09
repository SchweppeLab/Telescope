#pragma once

#include "DBManager.h"
#include "FastXCorr.h"
#include "FIManager.h"
#include "GlobalDefinitions.h"
#include "ParamsManager.h"

#include <thread>
#include <chrono>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace msclr::interop;

namespace TelescopeSharp {
	public ref class TScore {
	public:
		double score = 0;
		String^ peptide = "";
		String^ protein = "";
	};

	public ref class Telescope {
	public:
		Telescope();
		~Telescope();

		double GetMemUse(bool peptidoform);
		int GetPeptidoformCount();
		bool Init(String^ pFile);
		TScore^ Search(int thread, array<double>^ mz, array<double>^ intensity, double preMz, int preZ);

	private:

		bool ProcessDB();

		int maxScoreCount = 0;

		DBManager* dbm = nullptr;
		FIManager* fim = nullptr;
		FIMemoryManager* mm = nullptr;
		ParamsManager* params = nullptr;
		FastXCorr* xcorr = nullptr;
	};
}