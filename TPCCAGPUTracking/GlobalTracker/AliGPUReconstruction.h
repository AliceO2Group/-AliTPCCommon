#ifndef ALIGPURECONSTRUCTION_H
#define ALIGPURECONSTRUCTION_H

#include <cstddef>
#include <stdio.h>
#include <memory>
#include <fstream>

#include "AliHLTTRDDef.h"
#include "AliGPUCAParam.h"
#include "AliGPUCASettings.h"
#include "AliGPUCAOutputControl.h"

#include "AliHLTTPCCAClusterData.h"
#include "AliHLTTPCCASliceOutput.h"
#include "AliHLTTPCCATracker.h"
#include "AliHLTTPCGMMerger.h"

class AliHLTTPCCASliceOutput;
class AliHLTTPCCASliceOutTrack;
class AliHLTTPCCASliceOutCluster;
class AliHLTTPCGMMergedTrack;
class AliHLTTPCGMMergedTrackHit;
class AliHLTTRDTrackletWord;
class AliHLTTPCClusterMCLabel;
class AliHLTTPCCAMCInfo;
class AliHLTTRDTracker;
class AliHLTTPCCAGPUTracker;
struct AliHLTTPCRawCluster;
struct ClusterNativeAccessExt;
struct AliHLTTRDTrackletLabels;
class AliGPUCADisplay;
class AliGPUCAQA;

namespace o2 { namespace ITS { class TrackerTraits; class VertexerTraits; }}
namespace o2 { namespace trd { class TRDGeometryFlat; }}
namespace o2 { namespace TPC { struct ClusterNativeAccessFullTPC; struct ClusterNative; }}
namespace ali_tpc_common { namespace tpc_fast_transformation { class TPCFastTransform; }}
using TPCFastTransform = ali_tpc_common::tpc_fast_transformation::TPCFastTransform;

class AliGPUReconstruction
{
private:
	class LibraryLoader;
	std::shared_ptr<LibraryLoader> mMyLib = nullptr; //This must be the first member to ensure correct destructor order!
	
public:
	virtual ~AliGPUReconstruction();
	AliGPUReconstruction(const AliGPUReconstruction&) = delete;
	AliGPUReconstruction& operator=(const AliGPUReconstruction&) = delete;
	
	//General definitions
	constexpr static size_t MIN_ALIGNMENT = 64;
	constexpr static unsigned int NSLICES = 36;

	//Definition of the Geometry: Are we AliRoot or O2
	enum GeometryType : unsigned int {RESERVED_GEOMETRY = 0, ALIROOT = 1, O2 = 2};
	static constexpr const char* const GEOMETRY_TYPE_NAMES[] = {"INVALID", "ALIROOT", "O2"};
	#ifdef GPUCA_TPC_GEOMETRY_O2
		static constexpr GeometryType geometryType = O2;
	#else
		static constexpr GeometryType geometryType = ALIROOT;
	#endif
	
	enum DeviceType : unsigned int {INVALID_DEVICE = 0, CPU = 1, CUDA = 2, HIP = 3, OCL = 4};
	static constexpr const char* const DEVICE_TYPE_NAMES[] = {"INVALID", "CPU", "CUDA", "HIP", "OCL"};
	static DeviceType GetDeviceType(const char* type);

	//Functionality to create an instance of AliGPUReconstruction for the desired device
	static AliGPUReconstruction* CreateInstance(const AliGPUCASettingsProcessing& cfg);
	static AliGPUReconstruction* CreateInstance(DeviceType type = CPU, bool forceType = true);
	static AliGPUReconstruction* CreateInstance(int type, bool forceType) {return CreateInstance((DeviceType) type, forceType);}
	static AliGPUReconstruction* CreateInstance(const char* type, bool forceType);
	
	int Init();
	int Finalize();
	
	//Structures for input and output data
	struct InOutPointers
	{
		InOutPointers() : mcLabelsTPC(nullptr), nMCLabelsTPC(0), mcInfosTPC(nullptr), nMCInfosTPC(0),
			mergedTracks(nullptr), nMergedTracks(0), mergedTrackHits(nullptr), nMergedTrackHits(0),
			trdTracks(nullptr), nTRDTracks(0), trdTracklets(nullptr), nTRDTracklets(0), trdTrackletsMC(nullptr),
			nTRDTrackletsMC(0)
		{}
		InOutPointers(const InOutPointers&) = default;
		
		const AliHLTTPCCAClusterData::Data* clusterData[NSLICES];
		unsigned int nClusterData[NSLICES];
		const AliHLTTPCRawCluster* rawClusters[NSLICES];
		unsigned int nRawClusters[NSLICES];
		const o2::TPC::ClusterNativeAccessFullTPC* clustersNative;
		const AliHLTTPCCASliceOutTrack* sliceOutTracks[NSLICES];
		unsigned int nSliceOutTracks[NSLICES];
		const AliHLTTPCCASliceOutCluster* sliceOutClusters[NSLICES];
		unsigned int nSliceOutClusters[NSLICES];
		const AliHLTTPCClusterMCLabel* mcLabelsTPC;
		unsigned int nMCLabelsTPC;
		const AliHLTTPCCAMCInfo* mcInfosTPC;
		unsigned int nMCInfosTPC;
		const AliHLTTPCGMMergedTrack* mergedTracks;
		unsigned int nMergedTracks;
		const AliHLTTPCGMMergedTrackHit* mergedTrackHits;
		unsigned int nMergedTrackHits;
		const HLTTRDTrack* trdTracks;
		unsigned int nTRDTracks;
		const AliHLTTRDTrackletWord* trdTracklets;
		unsigned int nTRDTracklets;
		const AliHLTTRDTrackletLabels* trdTrackletsMC;
		unsigned int nTRDTrackletsMC;
	} mIOPtrs;

	struct InOutMemory
	{
		InOutMemory();
		~InOutMemory();
		InOutMemory(AliGPUReconstruction::InOutMemory&&);
		InOutMemory& operator=(InOutMemory&&);
		
		std::unique_ptr<AliHLTTPCCAClusterData::Data[]> clusterData[NSLICES];
		std::unique_ptr<AliHLTTPCRawCluster[]> rawClusters[NSLICES];
		std::unique_ptr<o2::TPC::ClusterNative[]> clustersNative[NSLICES * GPUCA_ROW_COUNT];
		std::unique_ptr<AliHLTTPCCASliceOutTrack[]> sliceOutTracks[NSLICES];
		std::unique_ptr<AliHLTTPCCASliceOutCluster[]> sliceOutClusters[NSLICES];
		std::unique_ptr<AliHLTTPCClusterMCLabel[]> mcLabelsTPC;
		std::unique_ptr<AliHLTTPCCAMCInfo[]> mcInfosTPC;
		std::unique_ptr<AliHLTTPCGMMergedTrack[]> mergedTracks;
		std::unique_ptr<AliHLTTPCGMMergedTrackHit[]> mergedTrackHits;
		std::unique_ptr<HLTTRDTrack[]> trdTracks;
		std::unique_ptr<AliHLTTRDTrackletWord[]> trdTracklets;
		std::unique_ptr<AliHLTTRDTrackletLabels[]> trdTrackletsMC;
	} mIOMem;
	
	//Functionality to dump and read input / output data
	enum InOutPointerType : unsigned int {CLUSTER_DATA = 0, SLICE_OUT_TRACK = 1, SLICE_OUT_CLUSTER = 2, MC_LABEL_TPC = 3, MC_INFO_TPC = 4, MERGED_TRACK = 5, MERGED_TRACK_HIT = 6, TRD_TRACK = 7, TRD_TRACKLET = 8, RAW_CLUSTERS = 9, CLUSTERS_NATIVE = 10, TRD_TRACKLET_MC = 11};
	static constexpr const char* const IOTYPENAMES[] = {"TPC Clusters", "TPC Slice Tracks", "TPC Slice Track Clusters", "TPC Cluster MC Labels", "TPC Track MC Informations", "TPC Tracks", "TPC Track Clusters", "TRD Tracks", "TRD Tracklets", "Raw Clusters", "ClusterNative", "TRD Tracklet MC Labels"};

	void ClearIOPointers();
	void AllocateIOMemory();
	void DumpData(const char* filename);
	int ReadData(const char* filename);
	void DumpSettings(const char* dir = "");
	void ReadSettings(const char* dir = "");
	
	//Helpers for memory allocation
	static inline size_t getAlignment(size_t addr, size_t alignment = MIN_ALIGNMENT)
	{
		if (alignment <= 1) return 0;
		size_t mod = addr & (alignment - 1);
		if (mod == 0) return 0;
		return (alignment - mod);
	}
	static inline size_t getAlignment(void* addr, size_t alignment = MIN_ALIGNMENT)
	{
		return(getAlignment(reinterpret_cast<size_t>(addr), alignment));
	}
	template <class S> static inline S* getPointerWithAlignment(size_t& basePtr, size_t nEntries = 1, size_t alignment = MIN_ALIGNMENT)
	{
		if (basePtr == 0) basePtr = 1;
		basePtr += getAlignment(basePtr, std::max(alignof(S), alignment));
		S* retVal = (S*) (basePtr);
		basePtr += nEntries * sizeof(S);
		return retVal;
	}
	template <class S> static inline S* getPointerWithAlignment(void*& basePtr, size_t nEntries = 1, size_t alignment = MIN_ALIGNMENT)
	{
		return getPointerWithAlignment<S>(reinterpret_cast<size_t&>(basePtr), nEntries, alignment);
	}
	template <class S> static inline S* getPointerWithAlignmentNoUpdate(void* basePtr, size_t nEntries = 1, size_t alignment = MIN_ALIGNMENT)
	{
		return getPointerWithAlignment<S>(basePtr, nEntries, alignment);
	}
	
	template <class T, class S> static inline void computePointerWithAlignment(T*& basePtr, S*& objPtr, size_t nEntries = 1, bool runConstructor = false, size_t alignment = MIN_ALIGNMENT)
	{
		objPtr = getPointerWithAlignment<S>(reinterpret_cast<size_t&>(basePtr), nEntries, alignment);
		if (runConstructor)
		{
			for (size_t i = 0;i < nEntries;i++)
			{
				new (objPtr + i) S;
			}
		}
	}
	
	//Converter functions
	void ConvertNativeToClusterData();
	
	//Getters for external usage of tracker classes
	AliHLTTRDTracker* GetTRDTracker() {return mTRDTracker.get();}
	o2::ITS::TrackerTraits* GetITSTrackerTraits() {return mITSTrackerTraits.get();}
    o2::ITS::VertexerTraits* GetITSVertexerTraits() {return mITSVertexerTraits.get();}
 	AliHLTTPCCATracker* GetTPCSliceTrackers() {return mTPCSliceTrackersCPU;}
	const AliHLTTPCCATracker* GetTPCSliceTrackers() const {return mTPCSliceTrackersCPU;}
	const AliHLTTPCGMMerger& GetTPCMerger() const {return mTPCMergerCPU;}
	AliHLTTPCGMMerger& GetTPCMerger() {return mTPCMergerCPU;}
	AliGPUCADisplay* GetEventDisplay() {return mEventDisplay.get();}
	const AliGPUCAQA* GetQA() const {return mQA.get();}
	AliGPUCAQA* GetQA() {return mQA.get();}
	
	//Processing functions
	int RunStandalone();
	virtual int RunTPCTrackingSlices();
	virtual int RunTPCTrackingMerger();
	int RunTRDTracking();
	
	//Getters / setters for parameters
	DeviceType GetDeviceType() const {return (DeviceType) mProcessingSettings.deviceType;}
	bool IsGPU() const {return GetDeviceType() != INVALID_DEVICE && GetDeviceType() != CPU;}
	const AliGPUCAParam& GetParam() const {return mParam;}
	const TPCFastTransform* GetTPCTransform() const {return mTPCFastTransform.get();}
	const ClusterNativeAccessExt* GetClusterNativeAccessExt() const {return mClusterNativeAccess.get();}
	const AliGPUCASettingsEvent& GetEventSettings() const {return mEventSettings;}
	const AliGPUCASettingsProcessing& GetProcessingSettings() {return mProcessingSettings;}
	const AliGPUCASettingsDeviceProcessing& GetDeviceProcessingSettings() const {return mDeviceProcessingSettings;}
	bool IsInitialized() const {return mInitialized;}
	void SetSettings(float solenoidBz);
	void SetSettings(const AliGPUCASettingsEvent* settings, const AliGPUCASettingsRec* rec = nullptr, const AliGPUCASettingsDeviceProcessing* proc = nullptr);
	void SetTPCFastTransform(std::unique_ptr<TPCFastTransform> tpcFastTransform);
	void SetTRDGeometry(const o2::trd::TRDGeometryFlat& geo);
	void LoadClusterErrors();
	void SetResetTimers(bool reset) {mDeviceProcessingSettings.resetTimers = reset;}
	void SetOutputControl(const AliGPUCAOutputControl& v) {mOutputControl = v;}
	void SetOutputControl(void* ptr, size_t size);
	AliGPUCAOutputControl& OutputControl() {return mOutputControl;}
	const AliHLTTPCCASliceOutput** SliceOutput() const {return (const AliHLTTPCCASliceOutput**) &mSliceOutput;}
	
	const void* mConfigDisplay = nullptr;										//Abstract pointer to Standalone Display Configuration Structure
	const void* mConfigQA = nullptr;											//Abstract pointer to Standalone QA Configuration Structure
	
protected:
	AliGPUReconstruction(const AliGPUCASettingsProcessing& cfg);				//Constructor
	virtual int InitDevice();
	virtual int ExitDevice();
	
	//Private helper functions for reading / writing / allocating IO buffer from/to file
	template <class T> void DumpData(FILE* fp, const T* const* entries, const unsigned int* num, InOutPointerType type);
	template <class T> size_t ReadData(FILE* fp, const T** entries, unsigned int* num, std::unique_ptr<T[]>* mem, InOutPointerType type);
	template <class T> void AllocateIOMemoryHelper(unsigned int n, const T* &ptr, std::unique_ptr<T[]> &u);
	
	//Private helper functions to dump / load flat objects
	template <class T> void DumpFlatObjectToFile(const T* obj, const char* file);
	template <class T> std::unique_ptr<T> ReadFlatObjectFromFile(const char* file);
	template <class T> void DumpStructToFile(const T* obj, const char* file);
	template <class T> std::unique_ptr<T> ReadStructFromFile(const char* file);
	template <class T> void ReadStructFromFile(const char* file, T* obj);
	
	//Pointers to tracker classes
	std::unique_ptr<AliHLTTRDTracker> mTRDTracker;
	std::unique_ptr<o2::ITS::TrackerTraits> mITSTrackerTraits;
	std::unique_ptr<o2::ITS::VertexerTraits> mITSVertexerTraits;
	AliHLTTPCCATracker mTPCSliceTrackersCPU[NSLICES];
	AliHLTTPCGMMerger mTPCMergerCPU;
	AliHLTTPCCASliceOutput* mSliceOutput[NSLICES];
	AliHLTTPCCAClusterData mClusterData[NSLICES];
	
	AliGPUCAParam mParam;														//Reconstruction parameters
	AliGPUCASettingsEvent mEventSettings;										//Event Parameters
	AliGPUCASettingsProcessing mProcessingSettings;								//Processing Parameters (at constructor level)
	AliGPUCASettingsDeviceProcessing mDeviceProcessingSettings;					//Processing Parameters (at init level)
	AliGPUCAOutputControl mOutputControl;										//Controls the output of the individual components
	
	std::unique_ptr<AliGPUCADisplay> mEventDisplay;
	bool mDisplayRunning = false;
	std::unique_ptr<AliGPUCAQA> mQA;
	bool mQAInitialized = false;
	
	std::unique_ptr<TPCFastTransform> mTPCFastTransform;						//Global TPC fast transformation object
	std::unique_ptr<ClusterNativeAccessExt> mClusterNativeAccess;				//Internal memory for clusterNativeAccess
	std::unique_ptr<o2::trd::TRDGeometryFlat> mTRDGeometry;						//TRD Geometry
	
	bool mInitialized = false;
	std::ofstream mDebugFile;
	
	int mStatNEvents = 0;

private:
	//Helpers for loading device library via dlopen
	class LibraryLoader
	{
	public:
		~LibraryLoader();

	private:
		friend class AliGPUReconstruction;
		LibraryLoader(const char* lib, const char* func);
		LibraryLoader(const LibraryLoader&) CON_DELETE;
		const LibraryLoader& operator= (const LibraryLoader&) CON_DELETE;
		int LoadLibrary();
		int CloseLibrary();
		AliGPUReconstruction* GetPtr(const AliGPUCASettingsProcessing& cfg);
		
		const char* mLibName;
		const char* mFuncName;
		void* mGPULib;
		void* mGPUEntry;
	};
	static std::shared_ptr<LibraryLoader> sLibCUDA, sLibHIP, sLibOCL;
};

#endif
