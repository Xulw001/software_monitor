#include "file_monitor.h"

#include <fltKernel.h>

#include "install_monitor.h"
#include "proc_util.h"
#include "string_util.h"

namespace monitor {
namespace file {

PFLT_FILTER filter_ = nullptr;

NTSTATUS FLTAPI FilterUnload(FLT_FILTER_UNLOAD_FLAGS) {
    if (filter_) FltUnregisterFilter(filter_);
    filter_ = nullptr;
    return STATUS_SUCCESS;
}

static void NotifyFileCreated(PFLT_CALLBACK_DATA callback) {
    PFLT_FILE_NAME_INFORMATION file_name_info = nullptr;
    do {
        if (!NT_SUCCESS(FltGetFileNameInformation(
                callback,
                FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_NORMALIZED,
                &file_name_info))) {
            break;
        }

        if (!NT_SUCCESS(FltParseFileNameInformation(file_name_info))) {
            break;
        }

        rtl::wstring file_name;
        CopyUnicodeString(file_name, &file_name_info->Name);

        rtl::wstring proc_image_name;
        if (!GetCurrentProcessImageName(proc_image_name)) {
            break;
        }

        install::FileInstall(proc_image_name, file_name);
    } while (false);
    if (file_name_info) {
        FltReleaseFileNameInformation(file_name_info);
    }
}

FLT_POSTOP_CALLBACK_STATUS FLTAPI PostCreate(PFLT_CALLBACK_DATA callback,
                                             PCFLT_RELATED_OBJECTS, PVOID,
                                             FLT_POST_OPERATION_FLAGS) {
    if (NT_SUCCESS(callback->IoStatus.Status)) {
        if (callback->IoStatus.Information == FILE_CREATED) {
            NotifyFileCreated(callback);
        }
    }
    return FLT_POSTOP_FINISHED_PROCESSING;
}

static void NotifyFileLink(PFLT_CALLBACK_DATA callback,
                           PCFLT_RELATED_OBJECTS flt_obj) {
    PFLT_FILE_NAME_INFORMATION file_name_info = nullptr;
    do {
        PFILE_LINK_INFORMATION link_info =
            (PFILE_LINK_INFORMATION)
                callback->Iopb->Parameters.SetFileInformation.InfoBuffer;
        if (!NT_SUCCESS(FltGetDestinationFileNameInformation(
                flt_obj->Instance, flt_obj->FileObject,
                link_info->RootDirectory, link_info->FileName,
                link_info->FileNameLength,
                FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_NORMALIZED,
                &file_name_info))) {
            break;
        }

        rtl::wstring file_name;
        CopyUnicodeString(file_name, &file_name_info->Name);

        rtl::wstring proc_image_name;
        if (!GetCurrentProcessImageName(proc_image_name)) {
            break;
        }

        install::FileInstall(proc_image_name, file_name);
    } while (false);
    if (file_name_info) {
        FltReleaseFileNameInformation(file_name_info);
    }
}

static void NotifyFileRenamed(PFLT_CALLBACK_DATA callback,
                              PCFLT_RELATED_OBJECTS flt_obj) {
    PFLT_FILE_NAME_INFORMATION file_name_info = nullptr;
    do {
        PFILE_RENAME_INFORMATION rename_info =
            (PFILE_RENAME_INFORMATION)
                callback->Iopb->Parameters.SetFileInformation.InfoBuffer;
        if (!NT_SUCCESS(FltGetDestinationFileNameInformation(
                flt_obj->Instance, flt_obj->FileObject,
                rename_info->RootDirectory, rename_info->FileName,
                rename_info->FileNameLength,
                FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_NORMALIZED,
                &file_name_info))) {
            break;
        }

        rtl::wstring file_name;
        CopyUnicodeString(file_name, &file_name_info->Name);

        rtl::wstring proc_image_name;
        if (!GetCurrentProcessImageName(proc_image_name)) {
            break;
        }

        install::FileInstall(proc_image_name, file_name);
    } while (false);
    if (file_name_info) {
        FltReleaseFileNameInformation(file_name_info);
    }
}

FLT_POSTOP_CALLBACK_STATUS FLTAPI PostSetInfo(PFLT_CALLBACK_DATA callback,
                                              PCFLT_RELATED_OBJECTS flt_obj,
                                              PVOID, FLT_POST_OPERATION_FLAGS) {
    if (NT_SUCCESS(callback->IoStatus.Status)) {
        auto file_info_class =
            callback->Iopb->Parameters.SetFileInformation.FileInformationClass;
        switch (file_info_class) {
            case FileRenameInformation:
            case FileRenameInformationBypassAccessCheck:
            case FileRenameInformationEx:
            case FileRenameInformationExBypassAccessCheck:
                NotifyFileRenamed(callback, flt_obj);
                break;
            case FileLinkInformation:
            case FileLinkInformationBypassAccessCheck:
            case FileLinkInformationEx:
            case FileLinkInformationExBypassAccessCheck:
                NotifyFileLink(callback, flt_obj);
                break;
            default:
                break;
        }
    }
    return FLT_POSTOP_FINISHED_PROCESSING;
}

CONST FLT_OPERATION_REGISTRATION FilterCallback[] = {
    {IRP_MJ_CREATE, 0, nullptr, PostCreate},
    {IRP_MJ_SET_INFORMATION, FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,
     nullptr, PostSetInfo},
    {IRP_MJ_OPERATION_END}};

CONST FLT_REGISTRATION filter_registration = {sizeof(FLT_REGISTRATION),
                                              FLT_REGISTRATION_VERSION,
                                              0,
                                              nullptr,
                                              FilterCallback,
                                              FilterUnload,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr};

bool initialize(PDRIVER_OBJECT drv) {
    if (!NT_SUCCESS(FltRegisterFilter(drv, &filter_registration, &filter_)))
        return false;
    if (!NT_SUCCESS(FltStartFiltering(filter_))) return false;
    return true;
}
void finalize() { FilterUnload(0); }

}  // namespace file
}  // namespace monitor
