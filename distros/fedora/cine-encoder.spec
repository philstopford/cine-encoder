Summary:    Video encoder
Name:       cine-encoder
Version:    3.5.5
Release:    1%{?dist}
License:    GPLv3
URL:        https://github.com/CineEncoder/%{name}
Source0:    %{name}-%{version}.tar.xz

BuildRequires: cmake, gcc-c++, qt6-qtbase-devel, qt6-qtsvg-devel, qt6-qtmultimedia-devel, libmediainfo-devel
Requires: ffmpeg, mkvtoolnix

%description
Cine Encoder is an application, uses the FFmpeg, MKVToolNix and MediaInfo
utilities, that allows to convert media files while preserving HDR metadata.
Supported hardware encoding NVENC for codecs H265, H264 and Intel QSV
(for Windows and experimental for Linux) for codecs H264, MPEG-2.
The following encoding modes are implemented: H265, H264, VP9, MPEG-2,
XDCAM, DNxHR, ProRes.
Licence: GNU GPL v.3

%prep
%autosetup

%build
%cmake -DBUILD_TESTING=OFF
%cmake_build

%install
%cmake_install
install -Dm644 share/%{name}.desktop %{buildroot}%{_datadir}/applications/%{name}.desktop
install -Dm644 share/%{name}.png %{buildroot}%{_datadir}/icons/hicolor/64x64/apps/%{name}.png
install -Dm644 share/%{name}.wav %{buildroot}%{_datadir}/sounds/%{name}.wav

%files
%doc share/ABOUT
%license LICENSE
%{_bindir}/cine_encoder
%{_datadir}/applications/cine-encoder.desktop
%{_datadir}/icons/hicolor/64x64/apps/cine-encoder.png
%{_datadir}/sounds/cine-encoder.wav

%changelog
* Sat Apr 15 2023 Cine Encoder <depositmail@rambler.ru>
- Initial package for Fedora.
