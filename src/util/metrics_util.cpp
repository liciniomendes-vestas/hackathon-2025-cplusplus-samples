#include <chrono>
#include <thread>
#include <utility>

#include "metrics_util.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/provider.h"

namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace common = opentelemetry::common;
namespace metrics_api = opentelemetry::metrics;
namespace otlp_exporter = opentelemetry::exporter::otlp;

namespace internal_log = opentelemetry::sdk::common::internal_log;

void metrics_util::init(const char *endpoint) {
    auto exporterOptions = otlp_exporter::OtlpHttpMetricExporterOptions();
    exporterOptions.url = endpoint;
    auto exporter = otlp_exporter::OtlpHttpMetricExporterFactory::Create(exporterOptions);

    std::string version{"1.2.0"};
    std::string schema{"https://opentelemetry.io/schemas/1.2.0"};

    const auto resource = opentelemetry::sdk::resource::Resource::Create({
        {"service.name", "hackathon-cpp-sample"},
        {"job", "hackathon-cpp-sample"}
    });

    // Initialize and set the global MeterProvider
    metrics_sdk::PeriodicExportingMetricReaderOptions reader_options;
    reader_options.export_interval_millis = std::chrono::milliseconds(1000);
    reader_options.export_timeout_millis = std::chrono::milliseconds(500);

    auto reader = metrics_sdk::PeriodicExportingMetricReaderFactory::Create(std::move(exporter),
                                                                            reader_options);
    auto context = metrics_sdk::MeterContextFactory::Create( std::make_unique<opentelemetry::sdk::metrics::ViewRegistry>(), resource);
    context->AddMetricReader(std::move(reader));

    auto u_provider = metrics_sdk::MeterProviderFactory::Create(std::move(context));
    const std::shared_ptr<opentelemetry::metrics::MeterProvider> provider(std::move(u_provider));

    metrics_sdk::Provider::SetMeterProvider(provider);
}

void metrics_util::cleanUpMetrics() {
    std::shared_ptr<metrics_api::MeterProvider> none;
    metrics_sdk::Provider::SetMeterProvider(none);
}

void metrics_util::writeCounter(const std::string &name, const double value) {
    const auto provider = metrics_api::Provider::GetMeterProvider();
    const opentelemetry::nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name);
    const auto counter = meter->CreateDoubleCounter(name);

    counter->Add(value);
}
